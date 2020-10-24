#pragma once

//# to c++ convert from [https://raw.githubusercontent.com/eddyem/fits_filter/master/median.c]

// FOR medianXX:
// Copyright (c) 1998 Nicolas Devillard. Public domain.

/*
 * median.c
 *
 * Copyright 2015 Edward V. Emelianov <eddy@sao.ru, edward.emelianoff@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 */

// TODO: resolve problem with borders


#include <!cpp/common.h>
// #include <!cpp/newKeywords.h>
#include <!cpp/TestRunner.h>


namespace Filter { namespace Median { namespace details { 

template<typename Item, int nSize> struct Filter;

template<typename Item, int nSize> 
struct Filter {
	Item filter(Item (&p)[nSize]);
};

template<typename T> void ELEM_SWAP(T &a, T &b) {
	swap(a, b);
}
template<typename T> void PIX_SORT_(T &a, T &b) {
	if(a > b) ELEM_SWAP(a, b);
}

#define PIX_SORT(aArg, bArg) do { Median::details::PIX_SORT_(p[(aArg)], p[(bArg)]); } while(0);

template<typename Item> struct Filter<Item, 2> { Item filter(Item (&p)[2]) {
	return (p[0] + p[1]) / 2;
}};
#if 1
template<typename Item> struct Filter<Item, 3> { Item filter(Item (&p)[3]) {
	PIX_SORT(0, 1); PIX_SORT(1, 2); PIX_SORT(0, 1);
	return(p[1]);
}};
template<typename Item> struct Filter<Item, 4> { Item filter(Item (&p)[4]) {
	PIX_SORT(0, 2); PIX_SORT(1, 3);
	PIX_SORT(0, 1); PIX_SORT(2, 3);
	return(p[1] + p[2]) / 2;
}};
template<typename Item> struct Filter<Item, 5> { Item filter(Item (&p)[5]) {
	PIX_SORT(0, 1); PIX_SORT(3, 4); PIX_SORT(0, 3);
	PIX_SORT(1, 4); PIX_SORT(1, 2); PIX_SORT(2, 3) ;
	PIX_SORT(1, 2);
	return(p[2]);
}};
// even values are from "FAST, EFFICIENT MEDIAN FILTERS WITH EVEN LENGTH WINDOWS", J.P. HAVLICEK, K.A. SAKADY, G.R.KATZ
template<typename Item> struct Filter<Item, 6> { Item filter(Item (&p)[6]) {
	PIX_SORT(1, 2); PIX_SORT(3, 4);
	PIX_SORT(0, 1); PIX_SORT(2, 3); PIX_SORT(4, 5);
	PIX_SORT(1, 2); PIX_SORT(3, 4);
	PIX_SORT(0, 1); PIX_SORT(2, 3); PIX_SORT(4, 5);
	PIX_SORT(1, 2); PIX_SORT(3, 4);
	return ( p[2] + p[3] ) / 2;
}};
template<typename Item> struct Filter<Item, 7> { Item filter(Item (&p)[7]) {
	PIX_SORT(0, 5); PIX_SORT(0, 3); PIX_SORT(1, 6);
	PIX_SORT(2, 4); PIX_SORT(0, 1); PIX_SORT(3, 5);
	PIX_SORT(2, 6); PIX_SORT(2, 3); PIX_SORT(3, 6);
	PIX_SORT(4, 5); PIX_SORT(1, 4); PIX_SORT(1, 3);
	PIX_SORT(3, 4); return (p[3]);
}};
// optimal Batcher's sort for 8 elements (http://myopen.googlecode.com/svn/trunk/gtkclient_tdt/include/fast_median.h)
template<typename Item> struct Filter<Item, 8> { Item filter(Item (&p)[8]) {
	PIX_SORT(0, 4); PIX_SORT(1, 5); PIX_SORT(2, 6);
	PIX_SORT(3, 7); PIX_SORT(0, 2); PIX_SORT(1, 3);
	PIX_SORT(4, 6); PIX_SORT(5, 7); PIX_SORT(2, 4);
	PIX_SORT(3, 5); PIX_SORT(0, 1); PIX_SORT(2, 3);
	PIX_SORT(4, 5); PIX_SORT(6, 7); PIX_SORT(1, 4);
	PIX_SORT(3, 6);
	return(p[3] + p[4]) / 2;
}};
template<typename Item> struct Filter<Item, 9> { Item filter(Item (&p)[9]) {
	PIX_SORT(1, 2); PIX_SORT(4, 5); PIX_SORT(7, 8);
	PIX_SORT(0, 1); PIX_SORT(3, 4); PIX_SORT(6, 7);
	PIX_SORT(1, 2); PIX_SORT(4, 5); PIX_SORT(7, 8);
	PIX_SORT(0, 3); PIX_SORT(5, 8); PIX_SORT(4, 7);
	PIX_SORT(3, 6); PIX_SORT(1, 4); PIX_SORT(2, 5);
	PIX_SORT(4, 7); PIX_SORT(4, 2); PIX_SORT(6, 4);
	PIX_SORT(4, 2); return(p[4]);
}};
template<typename Item> struct Filter<Item, 16> { Item filter(Item (&p)[16]) {
	PIX_SORT(0, 8); PIX_SORT(1, 9); PIX_SORT(2, 10); PIX_SORT(3, 11);
	PIX_SORT(4, 12); PIX_SORT(5, 13); PIX_SORT(6, 14); PIX_SORT(7, 15);
	PIX_SORT(0, 4); PIX_SORT(1, 5); PIX_SORT(2, 6); PIX_SORT(3, 7);
	PIX_SORT(8, 12); PIX_SORT(9, 13); PIX_SORT(10, 14); PIX_SORT(11, 15);
	PIX_SORT(4, 8); PIX_SORT(5, 9); PIX_SORT(6, 10); PIX_SORT(7, 11);
	PIX_SORT(0, 2); PIX_SORT(1, 3); PIX_SORT(4, 6); PIX_SORT(5, 7);
	PIX_SORT(8, 10); PIX_SORT(9, 11); PIX_SORT(12, 14); PIX_SORT(13, 15);
	PIX_SORT(2, 8); PIX_SORT(3, 9); PIX_SORT(6, 12); PIX_SORT(7, 13);
	PIX_SORT(2, 4); PIX_SORT(3, 5); PIX_SORT(6, 8); PIX_SORT(7, 9);
	PIX_SORT(10, 12); PIX_SORT(11, 13); PIX_SORT(0, 1); PIX_SORT(2, 3);
	PIX_SORT(4, 5); PIX_SORT(6, 7); PIX_SORT(8, 9); PIX_SORT(10, 11);
	PIX_SORT(12, 13); PIX_SORT(14, 15); PIX_SORT(1, 8); PIX_SORT(3, 10);
	PIX_SORT(5, 12); PIX_SORT(7, 14); PIX_SORT(5, 8); PIX_SORT(7, 10);
	return (p[7] + p[8]) / 2;
}};
template<typename Item> struct Filter<Item, 25> { Item filter(Item (&p)[25]) {
	PIX_SORT(0, 1)  ; PIX_SORT(3, 4)  ; PIX_SORT(2, 4) ;
	PIX_SORT(2, 3)  ; PIX_SORT(6, 7)  ; PIX_SORT(5, 7) ;
	PIX_SORT(5, 6)  ; PIX_SORT(9, 10) ; PIX_SORT(8, 10) ;
	PIX_SORT(8, 9)  ; PIX_SORT(12, 13); PIX_SORT(11, 13) ;
	PIX_SORT(11, 12); PIX_SORT(15, 16); PIX_SORT(14, 16) ;
	PIX_SORT(14, 15); PIX_SORT(18, 19); PIX_SORT(17, 19) ;
	PIX_SORT(17, 18); PIX_SORT(21, 22); PIX_SORT(20, 22) ;
	PIX_SORT(20, 21); PIX_SORT(23, 24); PIX_SORT(2, 5) ;
	PIX_SORT(3, 6)  ; PIX_SORT(0, 6)  ; PIX_SORT(0, 3) ;
	PIX_SORT(4, 7)  ; PIX_SORT(1, 7)  ; PIX_SORT(1, 4) ;
	PIX_SORT(11, 14); PIX_SORT(8, 14) ; PIX_SORT(8, 11) ;
	PIX_SORT(12, 15); PIX_SORT(9, 15) ; PIX_SORT(9, 12) ;
	PIX_SORT(13, 16); PIX_SORT(10, 16); PIX_SORT(10, 13) ;
	PIX_SORT(20, 23); PIX_SORT(17, 23); PIX_SORT(17, 20) ;
	PIX_SORT(21, 24); PIX_SORT(18, 24); PIX_SORT(18, 21) ;
	PIX_SORT(19, 22); PIX_SORT(8, 17) ; PIX_SORT(9, 18) ;
	PIX_SORT(0, 18) ; PIX_SORT(0, 9)  ; PIX_SORT(10, 19) ;
	PIX_SORT(1, 19) ; PIX_SORT(1, 10) ; PIX_SORT(11, 20) ;
	PIX_SORT(2, 20) ; PIX_SORT(2, 11) ; PIX_SORT(12, 21) ;
	PIX_SORT(3, 21) ; PIX_SORT(3, 12) ; PIX_SORT(13, 22) ;
	PIX_SORT(4, 22) ; PIX_SORT(4, 13) ; PIX_SORT(14, 23) ;
	PIX_SORT(5, 23) ; PIX_SORT(5, 14) ; PIX_SORT(15, 24) ;
	PIX_SORT(6, 24) ; PIX_SORT(6, 15) ; PIX_SORT(7, 16) ;
	PIX_SORT(7, 19) ; PIX_SORT(13, 21); PIX_SORT(15, 23) ;
	PIX_SORT(7, 13) ; PIX_SORT(7, 15) ; PIX_SORT(1, 9) ;
	PIX_SORT(3, 11) ; PIX_SORT(5, 17) ; PIX_SORT(11, 17) ;
	PIX_SORT(9, 17) ; PIX_SORT(4, 10) ; PIX_SORT(6, 12) ;
	PIX_SORT(7, 14) ; PIX_SORT(4, 6)  ; PIX_SORT(4, 7) ;
	PIX_SORT(12, 14); PIX_SORT(10, 14); PIX_SORT(6, 7) ;
	PIX_SORT(10, 12); PIX_SORT(6, 10) ; PIX_SORT(6, 17) ;
	PIX_SORT(12, 17); PIX_SORT(7, 17) ; PIX_SORT(7, 10) ;
	PIX_SORT(12, 18); PIX_SORT(7, 12) ; PIX_SORT(10, 18) ;
	PIX_SORT(12, 20); PIX_SORT(10, 20); PIX_SORT(10, 12) ;
	return (p[12]);
}};
#endif

#undef PIX_SORT

} //# namespace Median
} //# namespace details
} //# namespace Filter

namespace Filter { 
	template<typename Item, int nSize> Item median(Item (&p)[nSize]) {
		return (Median::details::Filter<Item, nSize>()).filter(p);
	}
} //# namespace Filter

#ifdef TESTRUNNER_ON
example("Filter::median") {
	using Filter::median;
	
	if(1) { int x[2] = { 6, 4 }; assert_eq(median(x), 5); } 
	if(1) { int x[3] = { 6, 4, 2 }; assert_eq(median(x), 4); } 
	if(1) { int x[4] = { 6, 4, 2, 8 }; assert_eq(median(x), 5); } 
	if(1) { int x[5] = { 6, 4, 2, 8, 9 }; assert_eq(median(x), 6); } 
	if(1) { int x[6] = { 6, 4, 2, 8, 9, 1 }; assert_eq(median(x), 5); } 
	if(1) { int x[7] = { 6, 4, 2, 8, 9, 1, 11 }; assert_eq(median(x), 6); } 
	if(1) { int x[8] = { 6, 4, 2, 8, 9, 1, 11, 0 }; assert_eq(median(x), 5); } 
	if(1) { int x[9] = { 6, 4, 2, 8, 9, 1, 11, 0, 12 }; assert_eq(median(x), 6); } 
	if(1) { int x[16] = { 6, 4, 2, 8, 9, 1, 11, 0, 12, -1, 13, -2, 14, -3, 15, -4 }; assert_eq(median(x), 5); } 
	if(1) { int x[25] = { 6, 4, 2, 8, 9, 1, 11, 0, 12, -1, 13, -2, 14, -3, 15, -4, 17, -5, 18, -6, 19, -7, 20, -8, 21 }; assert_eq(median(x), 6); } 
}
#endif
