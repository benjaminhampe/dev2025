/* Copyright 2016, Ableton AG, Berlin. All rights reserved.
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 *  If you would like to incorporate Link into a proprietary software application,
 *  please contact <link-devs@ableton.com>.
 */

#include <ableton/platforms/windows/Clock.hpp>

#include <chrono>
#include <cmath>
#include <cstdint>

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>

namespace ableton {
namespace platforms {
namespace windows {

Clock::Clock()
{
	LARGE_INTEGER frequency;
	QueryPerformanceFrequency(&frequency);
	mTicksToMicros = 1.0e6 / static_cast<double>(frequency.QuadPart);
}

Clock::Micros 
Clock::ticksToMicros(const Ticks ticks) const
{
    return Micros{std::llround(mTicksToMicros * static_cast<double>(ticks))};
}

Clock::Ticks 
Clock::microsToTicks(const Micros micros) const
{
	return static_cast<Ticks>(static_cast<double>(micros.count()) / mTicksToMicros);
}

Clock::Ticks 
Clock::ticks() const
{
	LARGE_INTEGER count;
	QueryPerformanceCounter(&count);
	return count.QuadPart;
}

std::chrono::microseconds 
Clock::micros() const
{
	return ticksToMicros(ticks());
}

} // namespace windows
} // namespace platforms
} // namespace ableton
