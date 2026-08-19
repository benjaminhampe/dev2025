set(LIB_NAME de_harfbuzz_14_3_1)
project(${LIB_NAME})

if (TARGET ${LIB_NAME})
    message("${BoldYellow}[benni] HEADER_GUARD: ${LIB_NAME}${ColourReset}")
else()
    set(LIB_HEADER_DIR ${CMAKE_CURRENT_LIST_DIR}/include)
    set(LIB_HEADERS
        ${LIB_HEADER_DIR}/hb.h
        ${LIB_HEADER_DIR}/hb-aat.h
        ${LIB_HEADER_DIR}/hb-aat-layout.h
        ${LIB_HEADER_DIR}/hb-blob.h
        ${LIB_HEADER_DIR}/hb-buffer.h
        ${LIB_HEADER_DIR}/hb-common.h
        ${LIB_HEADER_DIR}/hb-cplusplus.hh
        ${LIB_HEADER_DIR}/hb-deprecated.h
        ${LIB_HEADER_DIR}/hb-draw.h
        ${LIB_HEADER_DIR}/hb-face.h
        ${LIB_HEADER_DIR}/hb-font.h
        ${LIB_HEADER_DIR}/hb-ft.h
        ${LIB_HEADER_DIR}/hb-map.h
        ${LIB_HEADER_DIR}/hb-ot.h
        ${LIB_HEADER_DIR}/hb-ot-color.h
        ${LIB_HEADER_DIR}/hb-ot-deprecated.h
        ${LIB_HEADER_DIR}/hb-ot-fetch.h
        ${LIB_HEADER_DIR}/hb-ot-font.h
        ${LIB_HEADER_DIR}/hb-ot-layout.h
        ${LIB_HEADER_DIR}/hb-ot-math.h
        ${LIB_HEADER_DIR}/hb-ot-meta.h
        ${LIB_HEADER_DIR}/hb-ot-metrics.h
        ${LIB_HEADER_DIR}/hb-ot-name.h
        ${LIB_HEADER_DIR}/hb-ot-shape.h
        ${LIB_HEADER_DIR}/hb-ot-var.h
        ${LIB_HEADER_DIR}/hb-paint.h
        ${LIB_HEADER_DIR}/hb-script-list.h
        ${LIB_HEADER_DIR}/hb-set.h
        ${LIB_HEADER_DIR}/hb-shape.h
        ${LIB_HEADER_DIR}/hb-shape-plan.h
        ${LIB_HEADER_DIR}/hb-style.h
        ${LIB_HEADER_DIR}/hb-unicode.h
        ${LIB_HEADER_DIR}/hb-version.h
    )

    set(LIB_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/src)
    set(LIB_SOURCES
        ${LIB_SOURCE_DIR}/config/hb-features.h

        ${LIB_SOURCE_DIR}/harfbuzz.cc

        # ${LIB_SOURCE_DIR}/harfbuzz-subset.cc
        # ${LIB_SOURCE_DIR}/harfbuzz-world.cc

        # ${LIB_SOURCE_DIR}/hb-aat-layout.cc
        # ${LIB_SOURCE_DIR}/hb-aat-map.cc
        # ${LIB_SOURCE_DIR}/hb-blob.cc
        # ${LIB_SOURCE_DIR}/hb-buffer-serialize.cc
        # ${LIB_SOURCE_DIR}/hb-buffer-verify.cc
        # ${LIB_SOURCE_DIR}/hb-buffer.cc
        # ${LIB_SOURCE_DIR}/hb-cairo-utils.cc
        # ${LIB_SOURCE_DIR}/hb-cairo.cc
        # ${LIB_SOURCE_DIR}/hb-common.cc
        # ${LIB_SOURCE_DIR}/hb-coretext-font.cc
        # ${LIB_SOURCE_DIR}/hb-coretext-shape.cc
        # ${LIB_SOURCE_DIR}/hb-coretext.cc
        # ${LIB_SOURCE_DIR}/hb-depend.cc
        # ${LIB_SOURCE_DIR}/hb-directwrite-font.cc
        # ${LIB_SOURCE_DIR}/hb-directwrite-shape.cc
        # ${LIB_SOURCE_DIR}/hb-directwrite.cc
        # ${LIB_SOURCE_DIR}/hb-draw.cc
        # ${LIB_SOURCE_DIR}/hb-face-builder.cc
        # ${LIB_SOURCE_DIR}/hb-face.cc
        # ${LIB_SOURCE_DIR}/hb-fallback-shape.cc
        # ${LIB_SOURCE_DIR}/hb-font.cc
        # ${LIB_SOURCE_DIR}/hb-ft.cc
        # ${LIB_SOURCE_DIR}/hb-gdi.cc
        # ${LIB_SOURCE_DIR}/hb-glib.cc
        # ${LIB_SOURCE_DIR}/hb-gobject-structs.cc
        # ${LIB_SOURCE_DIR}/hb-gpu-draw.cc
        # ${LIB_SOURCE_DIR}/hb-gpu-paint.cc
        # ${LIB_SOURCE_DIR}/hb-gpu.cc
        # ${LIB_SOURCE_DIR}/hb-graphite2.cc
        # ${LIB_SOURCE_DIR}/hb-harfrust.cc
        # ${LIB_SOURCE_DIR}/hb-icu.cc
        # ${LIB_SOURCE_DIR}/hb-kbts.cc
        # ${LIB_SOURCE_DIR}/hb-map.cc
        # ${LIB_SOURCE_DIR}/hb-number.cc
        # ${LIB_SOURCE_DIR}/hb-ot-cff1-table.cc
        # ${LIB_SOURCE_DIR}/hb-ot-cff2-table.cc
        # ${LIB_SOURCE_DIR}/hb-ot-color.cc
        # ${LIB_SOURCE_DIR}/hb-ot-face.cc
        # ${LIB_SOURCE_DIR}/hb-ot-fetch.cc
        # ${LIB_SOURCE_DIR}/hb-ot-font.cc
        # ${LIB_SOURCE_DIR}/hb-ot-layout.cc
        # ${LIB_SOURCE_DIR}/hb-ot-map.cc
        # ${LIB_SOURCE_DIR}/hb-ot-math.cc
        # ${LIB_SOURCE_DIR}/hb-ot-meta.cc
        # ${LIB_SOURCE_DIR}/hb-ot-metrics.cc
        # ${LIB_SOURCE_DIR}/hb-ot-name.cc
        # ${LIB_SOURCE_DIR}/hb-ot-shape-fallback.cc
        # ${LIB_SOURCE_DIR}/hb-ot-shape-normalize.cc
        # ${LIB_SOURCE_DIR}/hb-ot-shape.cc
        # ${LIB_SOURCE_DIR}/hb-ot-shaper-arabic.cc
        # ${LIB_SOURCE_DIR}/hb-ot-shaper-default.cc
        # ${LIB_SOURCE_DIR}/hb-ot-shaper-hangul.cc
        # ${LIB_SOURCE_DIR}/hb-ot-shaper-hebrew.cc
        # ${LIB_SOURCE_DIR}/hb-ot-shaper-indic-table.cc
        # ${LIB_SOURCE_DIR}/hb-ot-shaper-indic.cc
        # ${LIB_SOURCE_DIR}/hb-ot-shaper-khmer.cc
        # ${LIB_SOURCE_DIR}/hb-ot-shaper-myanmar.cc
        # ${LIB_SOURCE_DIR}/hb-ot-shaper-syllabic.cc
        # ${LIB_SOURCE_DIR}/hb-ot-shaper-thai.cc
        # ${LIB_SOURCE_DIR}/hb-ot-shaper-use.cc
        # ${LIB_SOURCE_DIR}/hb-ot-shaper-vowel-constraints.cc
        # ${LIB_SOURCE_DIR}/hb-ot-tag.cc
        # ${LIB_SOURCE_DIR}/hb-ot-var.cc
        # ${LIB_SOURCE_DIR}/hb-outline.cc
        # ${LIB_SOURCE_DIR}/hb-paint-bounded.cc
        # ${LIB_SOURCE_DIR}/hb-paint-extents.cc
        # ${LIB_SOURCE_DIR}/hb-paint.cc
        # ${LIB_SOURCE_DIR}/hb-raster-draw.cc
        # ${LIB_SOURCE_DIR}/hb-raster-image.cc
        # ${LIB_SOURCE_DIR}/hb-raster-paint.cc
        # ${LIB_SOURCE_DIR}/hb-set.cc
        # ${LIB_SOURCE_DIR}/hb-shape-plan.cc
        # ${LIB_SOURCE_DIR}/hb-shape.cc
        # ${LIB_SOURCE_DIR}/hb-shaper.cc
        # ${LIB_SOURCE_DIR}/hb-static.cc
        # ${LIB_SOURCE_DIR}/hb-style.cc
        # ${LIB_SOURCE_DIR}/hb-subset-cff-common.cc
        # ${LIB_SOURCE_DIR}/hb-subset-cff1.cc
        # ${LIB_SOURCE_DIR}/hb-subset-cff2.cc
        # ${LIB_SOURCE_DIR}/hb-subset-input.cc
        # ${LIB_SOURCE_DIR}/hb-subset-instancer-iup.cc
        # ${LIB_SOURCE_DIR}/hb-subset-instancer-solver.cc
        # ${LIB_SOURCE_DIR}/hb-subset-plan-layout.cc
        # ${LIB_SOURCE_DIR}/hb-subset-plan-var.cc
        # ${LIB_SOURCE_DIR}/hb-subset-plan.cc
        # ${LIB_SOURCE_DIR}/hb-subset-serialize.cc
        # ${LIB_SOURCE_DIR}/hb-subset-table-cff.cc
        # ${LIB_SOURCE_DIR}/hb-subset-table-color.cc
        # ${LIB_SOURCE_DIR}/hb-subset-table-layout.cc
        # ${LIB_SOURCE_DIR}/hb-subset-table-other.cc
        # ${LIB_SOURCE_DIR}/hb-subset-table-var.cc
        # ${LIB_SOURCE_DIR}/hb-subset.cc
        # ${LIB_SOURCE_DIR}/hb-ucd.cc
        # ${LIB_SOURCE_DIR}/hb-unicode.cc
        # ${LIB_SOURCE_DIR}/hb-uniscribe.cc
        # ${LIB_SOURCE_DIR}/hb-vector-draw.cc
        # ${LIB_SOURCE_DIR}/hb-vector-paint-pdf.cc
        # ${LIB_SOURCE_DIR}/hb-vector-paint-svg.cc
        # ${LIB_SOURCE_DIR}/hb-vector-paint.cc
        # ${LIB_SOURCE_DIR}/hb-vector-path.cc
        # ${LIB_SOURCE_DIR}/hb-vector.cc
        # ${LIB_SOURCE_DIR}/hb-wasm-api.cc
        # ${LIB_SOURCE_DIR}/hb-wasm-shape.cc
        # ${LIB_SOURCE_DIR}/hb-zlib.cc

        # ${LIB_SOURCE_DIR}/graph/classdef-graph.hh
        # ${LIB_SOURCE_DIR}/graph/coverage-graph.hh
        # ${LIB_SOURCE_DIR}/graph/graph.hh
        # ${LIB_SOURCE_DIR}/graph/gsubgpos-context.cc
        # ${LIB_SOURCE_DIR}/graph/gsubgpos-context.hh
        # ${LIB_SOURCE_DIR}/graph/gsubgpos-graph.hh
        # ${LIB_SOURCE_DIR}/graph/ligature-graph.hh
        # ${LIB_SOURCE_DIR}/graph/markbasepos-graph.hh
        # ${LIB_SOURCE_DIR}/graph/pairpos-graph.hh
        # ${LIB_SOURCE_DIR}/graph/serialize.hh
        # ${LIB_SOURCE_DIR}/graph/split-helpers.hh
        # # ${LIB_SOURCE_DIR}/graph/test-classdef-graph.cc

        # ${LIB_SOURCE_DIR}/OT/Color/CBDT/CBDT.hh
        # ${LIB_SOURCE_DIR}/OT/Color/COLR/COLR.hh
        # ${LIB_SOURCE_DIR}/OT/Color/COLR/colrv1-closure.hh
        # ${LIB_SOURCE_DIR}/OT/Color/COLR/colrv1-depend.hh
        # ${LIB_SOURCE_DIR}/OT/Color/CPAL/CPAL.hh
        # ${LIB_SOURCE_DIR}/OT/Color/sbix/sbix.hh
        # ${LIB_SOURCE_DIR}/OT/Color/svg/svg.hh

        # ${LIB_SOURCE_DIR}/OT/glyf/composite-iter.hh
        # ${LIB_SOURCE_DIR}/OT/glyf/CompositeGlyph.hh
        # ${LIB_SOURCE_DIR}/OT/glyf/glyf-helpers.hh
        # ${LIB_SOURCE_DIR}/OT/glyf/glyf.hh
        # ${LIB_SOURCE_DIR}/OT/glyf/Glyph.hh
        # ${LIB_SOURCE_DIR}/OT/glyf/GlyphHeader.hh
        # ${LIB_SOURCE_DIR}/OT/glyf/loca.hh
        # ${LIB_SOURCE_DIR}/OT/glyf/path-builder.hh
        # ${LIB_SOURCE_DIR}/OT/glyf/SimpleGlyph.hh
        # ${LIB_SOURCE_DIR}/OT/glyf/SubsetGlyph.hh

        # ${LIB_SOURCE_DIR}/OT/Layout/types.hh
        # ${LIB_SOURCE_DIR}/OT/Layout/Common/Coverage.hh
        # ${LIB_SOURCE_DIR}/OT/Layout/Common/CoverageFormat1.hh
        # ${LIB_SOURCE_DIR}/OT/Layout/Common/CoverageFormat2.hh
        # ${LIB_SOURCE_DIR}/OT/Layout/Common/RangeRecord.hh
        # ${LIB_SOURCE_DIR}/OT/Layout/GDEF/GDEF.hh
        # ${LIB_SOURCE_DIR}/OT/Layout/GPOS/Anchor.hh
        # ${LIB_SOURCE_DIR}/OT/Layout/GPOS/AnchorFormat1.hh
        # ${LIB_SOURCE_DIR}/OT/Layout/GPOS/AnchorFormat2.hh
        # ${LIB_SOURCE_DIR}/OT/Layout/GPOS/AnchorFormat3.hh
        # ${LIB_SOURCE_DIR}/OT/Layout/GPOS/AnchorMatrix.hh
        # ${LIB_SOURCE_DIR}/OT/Layout/GPOS/ChainContextPos.hh
        # ${LIB_SOURCE_DIR}/OT/Layout/GPOS/Common.hh
        # ${LIB_SOURCE_DIR}/OT/Layout/GPOS/ContextPos.hh
        # ${LIB_SOURCE_DIR}/OT/Layout/GPOS/CursivePos.hh
        # ${LIB_SOURCE_DIR}/OT/Layout/GPOS/CursivePosFormat1.hh
        # ${LIB_SOURCE_DIR}/OT/Layout/GPOS/ExtensionPos.hh
        # ${LIB_SOURCE_DIR}/OT/Layout/GPOS/GPOS.hh
        # ${LIB_SOURCE_DIR}/OT/Layout/GPOS/LigatureArray.hh
        # ${LIB_SOURCE_DIR}/OT/Layout/GPOS/MarkArray.hh
        # ${LIB_SOURCE_DIR}/OT/Layout/GPOS/MarkBasePos.hh
        # ${LIB_SOURCE_DIR}/OT/Layout/GPOS/MarkBasePosFormat1.hh
        # ${LIB_SOURCE_DIR}/OT/Layout/GPOS/MarkLigPos.hh
        # ${LIB_SOURCE_DIR}/OT/Layout/GPOS/MarkLigPosFormat1.hh
        # ${LIB_SOURCE_DIR}/OT/Layout/GPOS/MarkMarkPos.hh
        # ${LIB_SOURCE_DIR}/OT/Layout/GPOS/MarkMarkPosFormat1.hh
        # ${LIB_SOURCE_DIR}/OT/Layout/GPOS/MarkRecord.hh
        # ${LIB_SOURCE_DIR}/OT/Layout/GPOS/PairPos.hh
        # ${LIB_SOURCE_DIR}/OT/Layout/GPOS/PairPosFormat1.hh
        # ${LIB_SOURCE_DIR}/OT/Layout/GPOS/PairPosFormat2.hh
        # ${LIB_SOURCE_DIR}/OT/Layout/GPOS/PairSet.hh
        # ${LIB_SOURCE_DIR}/OT/Layout/GPOS/PairValueRecord.hh
        # ${LIB_SOURCE_DIR}/OT/Layout/GPOS/PosLookup.hh
        # ${LIB_SOURCE_DIR}/OT/Layout/GPOS/PosLookupSubTable.hh
        # ${LIB_SOURCE_DIR}/OT/Layout/GPOS/SinglePos.hh
        # ${LIB_SOURCE_DIR}/OT/Layout/GPOS/SinglePosFormat1.hh
        # ${LIB_SOURCE_DIR}/OT/Layout/GPOS/SinglePosFormat2.hh
        # ${LIB_SOURCE_DIR}/OT/Layout/GPOS/ValueFormat.hh

        # ${LIB_SOURCE_DIR}/OT/Layout/GSUB/AlternateSet.hh
        # ${LIB_SOURCE_DIR}/OT/Layout/GSUB/AlternateSubst.hh
        # ${LIB_SOURCE_DIR}/OT/Layout/GSUB/AlternateSubstFormat1.hh
        # ${LIB_SOURCE_DIR}/OT/Layout/GSUB/ChainContextSubst.hh
        # ${LIB_SOURCE_DIR}/OT/Layout/GSUB/Common.hh
        # ${LIB_SOURCE_DIR}/OT/Layout/GSUB/ContextSubst.hh
        # ${LIB_SOURCE_DIR}/OT/Layout/GSUB/ExtensionSubst.hh
        # ${LIB_SOURCE_DIR}/OT/Layout/GSUB/GSUB.hh
        # ${LIB_SOURCE_DIR}/OT/Layout/GSUB/Ligature.hh
        # ${LIB_SOURCE_DIR}/OT/Layout/GSUB/LigatureSet.hh
        # ${LIB_SOURCE_DIR}/OT/Layout/GSUB/LigatureSubst.hh
        # ${LIB_SOURCE_DIR}/OT/Layout/GSUB/LigatureSubstFormat1.hh
        # ${LIB_SOURCE_DIR}/OT/Layout/GSUB/MultipleSubst.hh
        # ${LIB_SOURCE_DIR}/OT/Layout/GSUB/MultipleSubstFormat1.hh
        # ${LIB_SOURCE_DIR}/OT/Layout/GSUB/ReverseChainSingleSubst.hh
        # ${LIB_SOURCE_DIR}/OT/Layout/GSUB/ReverseChainSingleSubstFormat1.hh
        # ${LIB_SOURCE_DIR}/OT/Layout/GSUB/Sequence.hh
        # ${LIB_SOURCE_DIR}/OT/Layout/GSUB/SingleSubst.hh
        # ${LIB_SOURCE_DIR}/OT/Layout/GSUB/SingleSubstFormat1.hh
        # ${LIB_SOURCE_DIR}/OT/Layout/GSUB/SingleSubstFormat2.hh
        # ${LIB_SOURCE_DIR}/OT/Layout/GSUB/SubstLookup.hh
        # ${LIB_SOURCE_DIR}/OT/Layout/GSUB/SubstLookupSubTable.hh

        # ${LIB_SOURCE_DIR}/OT/name/name.hh

        # ${LIB_SOURCE_DIR}/OT/Var/VARC/coord-setter.hh
        # ${LIB_SOURCE_DIR}/OT/Var/VARC/VARC.cc
        # ${LIB_SOURCE_DIR}/OT/Var/VARC/VARC.hh

    )
    set(LIB_SOURCES_HEADERS
        ${LIB_SOURCE_DIR}/hb-aat-layout-ankr-table.hh
        ${LIB_SOURCE_DIR}/hb-aat-layout-bsln-table.hh
        ${LIB_SOURCE_DIR}/hb-aat-layout-common.hh
        ${LIB_SOURCE_DIR}/hb-aat-layout-feat-table.hh
        ${LIB_SOURCE_DIR}/hb-aat-layout-just-table.hh
        ${LIB_SOURCE_DIR}/hb-aat-layout-kerx-table.hh
        ${LIB_SOURCE_DIR}/hb-aat-layout-morx-table.hh
        ${LIB_SOURCE_DIR}/hb-aat-layout-opbd-table.hh
        ${LIB_SOURCE_DIR}/hb-aat-layout-trak-table.hh
        ${LIB_SOURCE_DIR}/hb-aat-layout.hh
        ${LIB_SOURCE_DIR}/hb-aat-ltag-table.hh
        ${LIB_SOURCE_DIR}/hb-aat-map.hh
        ${LIB_SOURCE_DIR}/hb-algs.hh
        ${LIB_SOURCE_DIR}/hb-alloc-pool.hh
        ${LIB_SOURCE_DIR}/hb-array.hh
        ${LIB_SOURCE_DIR}/hb-atomic.hh
        ${LIB_SOURCE_DIR}/hb-bimap.hh
        ${LIB_SOURCE_DIR}/hb-bit-page.hh
        ${LIB_SOURCE_DIR}/hb-bit-set-invertible.hh
        ${LIB_SOURCE_DIR}/hb-bit-set.hh
        ${LIB_SOURCE_DIR}/hb-bit-vector.hh
        ${LIB_SOURCE_DIR}/hb-blob.hh
        ${LIB_SOURCE_DIR}/hb-buffer-deserialize-json.hh
        ${LIB_SOURCE_DIR}/hb-buffer-deserialize-text-glyphs.hh
        ${LIB_SOURCE_DIR}/hb-buffer-deserialize-text-unicode.hh
        ${LIB_SOURCE_DIR}/hb-buffer.hh
        ${LIB_SOURCE_DIR}/hb-cache.hh
        ${LIB_SOURCE_DIR}/hb-cairo-utils.hh
        ${LIB_SOURCE_DIR}/hb-cairo.h
        ${LIB_SOURCE_DIR}/hb-cff-interp-common.hh
        ${LIB_SOURCE_DIR}/hb-cff-interp-cs-common.hh
        ${LIB_SOURCE_DIR}/hb-cff-interp-dict-common.hh
        ${LIB_SOURCE_DIR}/hb-cff-specializer.hh
        ${LIB_SOURCE_DIR}/hb-cff-width-optimizer.hh
        ${LIB_SOURCE_DIR}/hb-cff1-interp-cs.hh
        ${LIB_SOURCE_DIR}/hb-cff2-interp-cs.hh
        ${LIB_SOURCE_DIR}/hb-config.hh
        ${LIB_SOURCE_DIR}/hb-coretext.h
        ${LIB_SOURCE_DIR}/hb-coretext.hh
        ${LIB_SOURCE_DIR}/hb-debug.hh
        ${LIB_SOURCE_DIR}/hb-decycler.hh
        ${LIB_SOURCE_DIR}/hb-depend-data.hh
        ${LIB_SOURCE_DIR}/hb-depend.hh
        ${LIB_SOURCE_DIR}/hb-directwrite.h
        ${LIB_SOURCE_DIR}/hb-directwrite.hh
        ${LIB_SOURCE_DIR}/hb-dispatch.hh
        ${LIB_SOURCE_DIR}/hb-draw.hh
        ${LIB_SOURCE_DIR}/hb-face.hh
        ${LIB_SOURCE_DIR}/hb-font.hh
        ${LIB_SOURCE_DIR}/hb-fontations.h
        ${LIB_SOURCE_DIR}/hb-free-pool.hh
        ${LIB_SOURCE_DIR}/hb-ft-colr.hh
        ${LIB_SOURCE_DIR}/hb-gdi.h
        ${LIB_SOURCE_DIR}/hb-geometry.hh
        ${LIB_SOURCE_DIR}/hb-glib.h
        ${LIB_SOURCE_DIR}/hb-gobject-structs.h
        ${LIB_SOURCE_DIR}/hb-gobject.h
        ${LIB_SOURCE_DIR}/hb-gpu-cu2qu.hh
        ${LIB_SOURCE_DIR}/hb-gpu-draw-fragment-glsl.hh
        ${LIB_SOURCE_DIR}/hb-gpu-draw-fragment-hlsl.hh
        ${LIB_SOURCE_DIR}/hb-gpu-draw-fragment-msl.hh
        ${LIB_SOURCE_DIR}/hb-gpu-draw-fragment-wgsl.hh
        ${LIB_SOURCE_DIR}/hb-gpu-draw.hh
        ${LIB_SOURCE_DIR}/hb-gpu-fragment-glsl.hh
        ${LIB_SOURCE_DIR}/hb-gpu-fragment-hlsl.hh
        ${LIB_SOURCE_DIR}/hb-gpu-fragment-msl.hh
        ${LIB_SOURCE_DIR}/hb-gpu-fragment-wgsl.hh
        ${LIB_SOURCE_DIR}/hb-gpu-paint-fragment-glsl.hh
        ${LIB_SOURCE_DIR}/hb-gpu-paint-fragment-hlsl.hh
        ${LIB_SOURCE_DIR}/hb-gpu-paint-fragment-msl.hh
        ${LIB_SOURCE_DIR}/hb-gpu-paint-fragment-wgsl.hh
        ${LIB_SOURCE_DIR}/hb-gpu-paint.hh
        ${LIB_SOURCE_DIR}/hb-gpu-vertex-glsl.hh
        ${LIB_SOURCE_DIR}/hb-gpu-vertex-hlsl.hh
        ${LIB_SOURCE_DIR}/hb-gpu-vertex-msl.hh
        ${LIB_SOURCE_DIR}/hb-gpu-vertex-wgsl.hh
        ${LIB_SOURCE_DIR}/hb-gpu.h
        ${LIB_SOURCE_DIR}/hb-graphite2.h
        ${LIB_SOURCE_DIR}/hb-icu.h
        ${LIB_SOURCE_DIR}/hb-iter.hh
        ${LIB_SOURCE_DIR}/hb-kern.hh
        ${LIB_SOURCE_DIR}/hb-limits.hh
        ${LIB_SOURCE_DIR}/hb-machinery.hh
        ${LIB_SOURCE_DIR}/hb-map.hh
        ${LIB_SOURCE_DIR}/hb-meta.hh
        ${LIB_SOURCE_DIR}/hb-ms-feature-ranges.hh
        ${LIB_SOURCE_DIR}/hb-multimap.hh
        ${LIB_SOURCE_DIR}/hb-mutex.hh
        ${LIB_SOURCE_DIR}/hb-null.hh
        ${LIB_SOURCE_DIR}/hb-number-parser.hh
        ${LIB_SOURCE_DIR}/hb-number.hh
        ${LIB_SOURCE_DIR}/hb-object.hh
        ${LIB_SOURCE_DIR}/hb-open-file.hh
        ${LIB_SOURCE_DIR}/hb-open-type.hh
        ${LIB_SOURCE_DIR}/hb-ot-cff-common.hh
        ${LIB_SOURCE_DIR}/hb-ot-cff1-std-str.hh
        ${LIB_SOURCE_DIR}/hb-ot-cff1-table.hh
        ${LIB_SOURCE_DIR}/hb-ot-cff2-table.hh
        ${LIB_SOURCE_DIR}/hb-ot-cmap-table.hh
        ${LIB_SOURCE_DIR}/hb-ot-face-table-list.hh
        ${LIB_SOURCE_DIR}/hb-ot-face.hh
        ${LIB_SOURCE_DIR}/hb-ot-gasp-table.hh
        ${LIB_SOURCE_DIR}/hb-ot-glyf-table.hh
        ${LIB_SOURCE_DIR}/hb-ot-hdmx-table.hh
        ${LIB_SOURCE_DIR}/hb-ot-head-table.hh
        ${LIB_SOURCE_DIR}/hb-ot-hhea-table.hh
        ${LIB_SOURCE_DIR}/hb-ot-hmtx-table.hh
        ${LIB_SOURCE_DIR}/hb-ot-kern-table.hh
        ${LIB_SOURCE_DIR}/hb-ot-layout-base-table.hh
        ${LIB_SOURCE_DIR}/hb-ot-layout-common.hh
        ${LIB_SOURCE_DIR}/hb-ot-layout-gdef-table.hh
        ${LIB_SOURCE_DIR}/hb-ot-layout-gpos-table.hh
        ${LIB_SOURCE_DIR}/hb-ot-layout-gsub-table.hh
        ${LIB_SOURCE_DIR}/hb-ot-layout-gsubgpos.hh
        ${LIB_SOURCE_DIR}/hb-ot-layout-jstf-table.hh
        ${LIB_SOURCE_DIR}/hb-ot-layout.hh
        ${LIB_SOURCE_DIR}/hb-ot-map.hh
        ${LIB_SOURCE_DIR}/hb-ot-math-table.hh
        ${LIB_SOURCE_DIR}/hb-ot-maxp-table.hh
        ${LIB_SOURCE_DIR}/hb-ot-meta-table.hh
        ${LIB_SOURCE_DIR}/hb-ot-metrics.hh
        ${LIB_SOURCE_DIR}/hb-ot-name-language-static.hh
        ${LIB_SOURCE_DIR}/hb-ot-name-language.hh
        ${LIB_SOURCE_DIR}/hb-ot-name-table.hh
        ${LIB_SOURCE_DIR}/hb-ot-os2-table.hh
        ${LIB_SOURCE_DIR}/hb-ot-os2-unicode-ranges.hh
        ${LIB_SOURCE_DIR}/hb-ot-post-macroman.hh
        ${LIB_SOURCE_DIR}/hb-ot-post-table-v2subset.hh
        ${LIB_SOURCE_DIR}/hb-ot-post-table.hh
        ${LIB_SOURCE_DIR}/hb-ot-shape-fallback.hh
        ${LIB_SOURCE_DIR}/hb-ot-shape-normalize.hh
        ${LIB_SOURCE_DIR}/hb-ot-shape.hh
        ${LIB_SOURCE_DIR}/hb-ot-shaper-arabic-fallback.hh
        ${LIB_SOURCE_DIR}/hb-ot-shaper-arabic-joining-list.hh
        ${LIB_SOURCE_DIR}/hb-ot-shaper-arabic-pua.hh
        ${LIB_SOURCE_DIR}/hb-ot-shaper-arabic-table.hh
        ${LIB_SOURCE_DIR}/hb-ot-shaper-arabic-win1256.hh
        ${LIB_SOURCE_DIR}/hb-ot-shaper-arabic.hh
        ${LIB_SOURCE_DIR}/hb-ot-shaper-indic-machine.hh
        ${LIB_SOURCE_DIR}/hb-ot-shaper-indic.hh
        ${LIB_SOURCE_DIR}/hb-ot-shaper-khmer-machine.hh
        ${LIB_SOURCE_DIR}/hb-ot-shaper-myanmar-machine.hh
        ${LIB_SOURCE_DIR}/hb-ot-shaper-syllabic.hh
        ${LIB_SOURCE_DIR}/hb-ot-shaper-use-machine.hh
        ${LIB_SOURCE_DIR}/hb-ot-shaper-use-table.hh
        ${LIB_SOURCE_DIR}/hb-ot-shaper-vowel-constraints.hh
        ${LIB_SOURCE_DIR}/hb-ot-shaper.hh
        ${LIB_SOURCE_DIR}/hb-ot-stat-table.hh
        ${LIB_SOURCE_DIR}/hb-ot-tag-table.hh
        ${LIB_SOURCE_DIR}/hb-ot-var-avar-table.hh
        ${LIB_SOURCE_DIR}/hb-ot-var-common.hh
        ${LIB_SOURCE_DIR}/hb-ot-var-cvar-table.hh
        ${LIB_SOURCE_DIR}/hb-ot-var-fvar-table.hh
        ${LIB_SOURCE_DIR}/hb-ot-var-gvar-table.hh
        ${LIB_SOURCE_DIR}/hb-ot-var-hvar-table.hh
        ${LIB_SOURCE_DIR}/hb-ot-var-mvar-table.hh
        ${LIB_SOURCE_DIR}/hb-ot-var-varc-table.hh
        ${LIB_SOURCE_DIR}/hb-ot-vorg-table.hh
        ${LIB_SOURCE_DIR}/hb-outline.hh
        ${LIB_SOURCE_DIR}/hb-paint-bounded.hh
        ${LIB_SOURCE_DIR}/hb-paint-extents.hh
        ${LIB_SOURCE_DIR}/hb-paint.hh
        ${LIB_SOURCE_DIR}/hb-priority-queue.hh
        ${LIB_SOURCE_DIR}/hb-raster-image.hh
        ${LIB_SOURCE_DIR}/hb-raster-paint.hh
        ${LIB_SOURCE_DIR}/hb-raster.h
        ${LIB_SOURCE_DIR}/hb-raster.hh
        ${LIB_SOURCE_DIR}/hb-repacker.hh
        ${LIB_SOURCE_DIR}/hb-sanitize.hh
        ${LIB_SOURCE_DIR}/hb-serialize.hh
        ${LIB_SOURCE_DIR}/hb-set-digest.hh
        ${LIB_SOURCE_DIR}/hb-set.hh
        ${LIB_SOURCE_DIR}/hb-shape-plan.hh
        ${LIB_SOURCE_DIR}/hb-shaper-impl.hh
        ${LIB_SOURCE_DIR}/hb-shaper-list.hh
        ${LIB_SOURCE_DIR}/hb-shaper.hh
        ${LIB_SOURCE_DIR}/hb-string-array.hh
        ${LIB_SOURCE_DIR}/hb-subset-accelerator.hh
        ${LIB_SOURCE_DIR}/hb-subset-cff-common.hh
        ${LIB_SOURCE_DIR}/hb-subset-cff2-to-cff1.hh
        ${LIB_SOURCE_DIR}/hb-subset-depend.h
        ${LIB_SOURCE_DIR}/hb-subset-input.hh
        ${LIB_SOURCE_DIR}/hb-subset-instancer-iup.hh
        ${LIB_SOURCE_DIR}/hb-subset-instancer-solver.hh
        ${LIB_SOURCE_DIR}/hb-subset-plan-member-list.hh
        ${LIB_SOURCE_DIR}/hb-subset-plan.hh
        ${LIB_SOURCE_DIR}/hb-subset-serialize.h
        ${LIB_SOURCE_DIR}/hb-subset-table.hh
        ${LIB_SOURCE_DIR}/hb-subset.h
        ${LIB_SOURCE_DIR}/hb-subset.hh
        ${LIB_SOURCE_DIR}/hb-ucd-table.hh
        ${LIB_SOURCE_DIR}/hb-unicode-emoji-table.hh
        ${LIB_SOURCE_DIR}/hb-unicode.hh
        ${LIB_SOURCE_DIR}/hb-uniscribe.h
        ${LIB_SOURCE_DIR}/hb-utf.hh
        ${LIB_SOURCE_DIR}/hb-vector-buf.hh
        ${LIB_SOURCE_DIR}/hb-vector-draw.hh
        ${LIB_SOURCE_DIR}/hb-vector-internal.hh
        ${LIB_SOURCE_DIR}/hb-vector-paint.hh
        ${LIB_SOURCE_DIR}/hb-vector-path.hh
        ${LIB_SOURCE_DIR}/hb-vector.h
        ${LIB_SOURCE_DIR}/hb-vector.hh
        ${LIB_SOURCE_DIR}/hb-wasm-api-blob.hh
        ${LIB_SOURCE_DIR}/hb-wasm-api-buffer.hh
        ${LIB_SOURCE_DIR}/hb-wasm-api-common.hh
        ${LIB_SOURCE_DIR}/hb-wasm-api-face.hh
        ${LIB_SOURCE_DIR}/hb-wasm-api-font.hh
        ${LIB_SOURCE_DIR}/hb-wasm-api-list.hh
        ${LIB_SOURCE_DIR}/hb-wasm-api-shape.hh
        ${LIB_SOURCE_DIR}/hb-wasm-api.h
        ${LIB_SOURCE_DIR}/hb-wasm-api.hh
        ${LIB_SOURCE_DIR}/hb-zlib.hh
        ${LIB_SOURCE_DIR}/hb.hh
    )


    if (BUILD_SHARED_LIBS)
        add_library(${LIB_NAME} SHARED "")
        message("${BoldBlue}[benni] DLL: ${LIB_NAME}${ColourReset}")
        # target_compile_definitions(${LIB_NAME} PUBLIC SB_CONFIG_DLL_IMPORT=1)
        # target_compile_definitions(${LIB_NAME} PRIVATE SB_CONFIG_DLL_EXPORT=1)
    else()
        add_library(${LIB_NAME} STATIC "")
        message("${BoldCyan}[benni] LIB: ${LIB_NAME}${ColourReset}")
    endif()

    target_sources(${LIB_NAME} PUBLIC ${LIB_HEADERS} PRIVATE ${LIB_SOURCES} ${LIB_SOURCES_HEADERS})

    target_include_directories(${LIB_NAME} PUBLIC ${LIB_HEADER_DIR})
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR})
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR}/config)
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR}/graph)
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR}/OT)
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR}/OT/Layout)
    target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR}/OT/Layout/GSUB)
    # target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR}/OT/Layout/GSUB)
    # target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR}/OT/Layout/GSUB)
    # target_include_directories(${LIB_NAME} PRIVATE ${LIB_SOURCE_DIR}/OT/Layout/GSUB)

    # target_compile_definitions(${LIB_NAME} PUBLIC SB_CONFIG_EXPERIMENTAL_TEXT_API=1)

    target_link_libraries(${LIB_NAME} PUBLIC de_freetype2)

    target_compile_definitions(${LIB_NAME} PUBLIC HAVE_UNISTD_H=1)
    # target_compile_definitions(${LIB_NAME} PUBLIC HAVE_SYS_MMAN_H=1)
    target_compile_definitions(${LIB_NAME} PUBLIC HAVE_STDBOOL_H=1)
    target_compile_definitions(${LIB_NAME} PUBLIC HAVE_PTHREAD=1)

    target_link_libraries(${LIB_NAME} PUBLIC pthread)

    # target_compile_options(${LIB_NAME} PUBLIC -Wno-deprecated-declarations)

endif()

# add_subdirectory(test)
