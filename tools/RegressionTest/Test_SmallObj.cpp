//
//Test the Small Object Allocator
// MKH
//

#include <Loki\SmallObj.h>

namespace
{
	struct Test : Loki::SmallObj
	{
	};
	void Test_SmallObject()
	{
		Test;
	}
}//ns anon

extern void Test_SmallObj()
{
	Test_SmallObject();
}