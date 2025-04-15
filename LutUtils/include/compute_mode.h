#ifndef __LUT_LIBRARY_COMPUTATION_MODE_CONSTANTS__
#define __LUT_LIBRARY_COMPUTATION_MODE_CONSTANTS__


constexpr bool isHighAccuracyModeEnabled (void) noexcept
{ 
#ifdef WITH_HIGH_ACCURACY_MODE
   return true; 
#else
   return false; 
#endif
}
 
#endif /* __LUT_LIBRARY_COMPUTATION_MODE_CONSTANTS__ */ 
