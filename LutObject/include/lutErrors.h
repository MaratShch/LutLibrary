#ifndef __LUT_LIBRARY_LUT_ERRORS__
#define __LUT_LIBRARY_LUT_ERRORS__

namespace LutErrorCode
{
	enum class LutState
	{
		AlreadyLoaded = -2,
		AsyncIoPended = -1,
		OK = 0,
		NotInitialized,
		ReadError = 10,
		WriteError,
		PrematureEndOfFile,
		LineError,
		UnknownOrRepeatedKeyword = 20,
		TitleMissingQuote,
		DomainBoundReversed,
		LutSizeOutOfRange,
		CouldNotParseTableData,
		GenericError = 100
	};
}

#endif /* __LUT_LIBRARY_LUT_ERRORS__ */