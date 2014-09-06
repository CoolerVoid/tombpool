#    Copyright (C) 2012 tombpool authors,
#    
#    This file is part of tombpool
#    
#    tombpool is free software: you can redistribute it and/or modify
#    it under the terms of the GNU Lesser General Public License as published by
#    the Free Software Foundation, either version 3 of the License, or
#    (at your option) any later version.
#
#    tombpool is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU Lesser General Public License for more details.
#    You should have received a copy of the GNU Lesser General Public License
#    along with this program.  If not, see <http://www.gnu.org/licenses/>.
CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

SET(CMAKE_C_FLAGS "-O2 -fstack-protector-all -Wl,-z,relro,-z,now -D_FORTIFY_SOURCE=2")

SET(tombpoolSources tombpool)
INCLUDE_DIRECTORIES("${CMAKE_CURRENT_SOURCE_DIR}")

ADD_LIBRARY(tombpool SHARED ${tombpoolSources})
SET_TARGET_PROPERTIES(tombpool PROPERTIES LINKER_LANGUAGE C)
TARGET_LINK_LIBRARIES(tombpool pthread)

ADD_LIBRARY(tombpool_static STATIC ${tombpoolSources})
SET_TARGET_PROPERTIES(tombpool_static PROPERTIES LINKER_LANGUAGE C)



