#pragma once
#ifndef __LOKITESTTYPES_H
#define __LOKITESTTYPES_H
//Types to use in the test
namespace Loki
	{
	namespace RegressionTest
		{
		struct Complex
			{
			float r,i;
			};
		struct Complex_Convertable
			{
			Complex_Convertable(){}
			Complex_Convertable(float x) : r(x), i(0) {}
			float r,i;
			};
		struct TwoWay
			{
			TwoWay(double x) : r(x) {}
			operator double(){return r;}
			double r;
			};
		struct SuperClass
			{
			};
		struct SubClass : SuperClass
			{
			};
		typedef TYPELIST_1(Loki::NullType) tyNullList;
		typedef TYPELIST_1(void) tyList1;
		typedef TYPELIST_2(int, double) tyList2;
		typedef TYPELIST_5(std::vector<int>, Loki::RegressionTest::Complex, void, int, double) tyList5;
		}
	}
#endif