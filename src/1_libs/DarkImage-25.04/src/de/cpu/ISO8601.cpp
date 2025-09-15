#include <de/cpu/ISO8601.h>
#include <chrono>
#include <iomanip>
#include <sstream>

namespace de {

std::string 
ISO8601::str(bool bDate, bool bTime, bool bNanos) const
{
	std::ostringstream o;
	if (bDate) o << d;
	if (bTime) o << "T" << t;
	if (bNanos) o << "." << ns;
	return o.str();
}

// static 
ISO8601 
ISO8601::fromNanoseconds(int64_t ns) 
{ 
	ISO8601 iso;
	
	// Convert 'chrono.nanoseconds to 'chrono.time_point'
	// Convert 'chrono.time_point' to 'time_t' (using to_time_t())
	// Convert 'time_t' to 'tm' (using gmtime())
	// auto time_c0 = std::chrono::system_clock::time_point(
	// 	std::chrono::nanoseconds( std::chrono::duration<int64_t, std::nano>(ns) ) );

	auto time_c0 = std::chrono::system_clock::time_point(
		std::chrono::duration_cast<std::chrono::system_clock::duration>(
			std::chrono::nanoseconds(ns)
		)
	);

	time_t time = std::chrono::system_clock::to_time_t(time_c0);
	struct tm tm;
#ifdef _WIN32
	gmtime_s(&tm, &time);
#else
	gmtime_r(&time, &tm);
#endif
	// o << std::put_time(&tm, "%F");
	
	// Use decomposed DateTime 'tm':
	std::ostringstream o; 
	o << std::setfill('0') << std::setw(4) << tm.tm_year << "-"
	  << std::setw(2) << tm.tm_mon << "-" << std::setw(2) << tm.tm_mday;
	iso.d = o.str();
	
	std::ostringstream p; 
	p << std::setfill('0') << std::setw(2) << tm.tm_hour << ":"
	  << std::setw(2) << tm.tm_min << ":" << std::setw(2) << tm.tm_sec;
	iso.t = p.str();
	
	//iso.ns = std::to_string(tp.time_since_epoch().count() % 1000000000);
	iso.ns = std::to_string(ns % 1000000000);
	
	return iso;
}

} // end namespace de
