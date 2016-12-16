/******************************************************************************
 testrand.cc

	Test random number code.

	Copyright (C) 1997 by Dustin Laurence.  All rights reserved.
	
	Base code generated by Codemill v0.1.0

 *****************************************************************************/

#include <JKLRand.h>
#include <iomanip>
#include <jAssert.h>

	JInt32 jKLInt32List[] =
		{
		0x12345678,
		0x75432777,
		0xcd305e6a,
		0x25dbfac1,
		0x4b5c952c,
		0x84de0e9b,
		0xe2aa733e,
		0xea0f8185,
		0xf2d08520,
		0xa63075ff,
		0x81cf8b52,
		0x207db289,
		0x00 // Sentinel
		};

	long jKLLongList[] =
		{
		0,
		2360,
		2785,
		8196,
		6679,
		3841,
		6218,
		3437,
		6400,
		5078,
		5818,
		0 // Sentinel
		};

/******************************************************************************
 main

 *****************************************************************************/

int
main()
{
	std::cout << "Beginning random number test.  No news is good news" << std::endl;

// Test UniformInt32, the basic generator method
	std::cout << std::setbase(16);
	JKLRand randGen( jKLInt32List[0] );
	JIndex i = 1;
	while (jKLInt32List[i] != 0)
		{
		JInt32 thisVal = randGen.UniformInt32();
		if (jKLInt32List[i] != thisVal)
			{
			std::cout << "   UniformInt32 failed on test value " << i << "; generated "
			     << thisVal << " instead of " << jKLInt32List[i] << std::endl;
			}
		i++;
		}

// Tests UniformClosedProb as well.
	std::cout << std::setbase(10);
	randGen.SetSeed( jKLLongList[0] );
	i=1;
	while (jKLLongList[i] != 0)
		{
		long thisVal = randGen.UniformLong(0, 10000);
		if (jKLLongList[i] != thisVal)
			{
			std::cout << "   UniformLong failed on test value " << i << "; generated "
			     << thisVal << " instead of " << jKLLongList[i] << std::endl;
			}
		i++;
		}

	for (i=1;i<=1000000;i++)
		{
		double thisVal = randGen.UniformClosedProb();
		if (thisVal < 0.0 || thisVal > 1.0)
			{
			std::cout << "   UniformClosedProb returned " << thisVal;
			}
		}

	for (i=1;i<=1000000;i++)
		{
		double thisVal = randGen.UniformDouble(-100.0, 100.0);
		if (thisVal < -100.0 || thisVal > 100.0)
			{
			std::cout << "   UniformDouble(-100.0, 100.0) returned " << thisVal;
			}
		}

	std::cout << "Finished random number test.  If nothing printed out, it passed" << std::endl;

	return 0;
}
