#include <algorithm>
#include <memory>
#include "common/alloc.h"
#include "graph/image_filter.h"

#include "frame.h"
#include "utils.h"

struct FilterExecutor::data {
	zimg::AlignedVector<char> ctx;
	zimg::AlignedVector<char> tmp;
};

void FilterExecutor::exec_grey(const zimg::graph::ImageFilter *filter, unsigned plane)
{
	auto src_buffer = m_src_frame->as_read_buffer(plane);
	auto dst_buffer = m_dst_frame->as_write_buffer(plane);

	auto attr = filter->get_image_attributes();
	unsigned step = filter->get_simultaneous_lines();

	filter->init_context(m_data->ctx.data(), plane);

	for (unsigned i = 0; i < attr.height; i += step) {
		filter->process(m_data->ctx.data(), &src_buffer, &dst_buffer, m_data->tmp.data(), i, 0, attr.width);
	}
}

void FilterExecutor::exec_color()
{
	auto attr = m_filter->get_image_attributes();
	unsigned step = m_filter->get_simultaneous_lines();

	m_filter->init_context(m_data->ctx.data(), 0);

	for (unsigned i = 0; i < attr.height; i += step) {
		m_filter->process(m_data->ctx.data(), m_src_frame->as_read_buffer(), m_dst_frame->as_write_buffer(), m_data->tmp.data(), i, 0, attr.width);
	}
}

FilterExecutor::FilterExecutor(const zimg::graph::ImageFilter *filter, const zimg::graph::ImageFilter *filter_uv, const ImageFrame *src_frame, ImageFrame *dst_frame) :
	m_data{ std::make_shared<data>() },
	m_filter{ filter },
	m_filter_uv{ filter_uv },
	m_src_frame{ src_frame },
	m_dst_frame{ dst_frame }
{
	filter_uv = filter_uv ? filter_uv : filter;

	m_data->ctx.resize(std::max(filter->get_context_size(), filter_uv->get_context_size()));
	m_data->tmp.resize(std::max(filter->get_tmp_size(0, dst_frame->width()),
	                            filter_uv->get_tmp_size(0, dst_frame->width())));
}

void FilterExecutor::operator()()
{
	zimg::graph::ImageFilter::filter_flags flags = m_filter->get_flags();

	if (!flags.color) {
		unsigned planes = m_dst_frame->planes();

		for (unsigned p = 0; p < planes; ++p) {
			const zimg::graph::ImageFilter *filter = (m_filter_uv && (p == 1 || p == 2)) ? m_filter_uv : m_filter;
			exec_grey(filter, p);
		}
	} else {
		exec_color();
	}
}
