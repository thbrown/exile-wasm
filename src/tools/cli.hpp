
#ifndef __EMSCRIPTEN__
	#include <boost/optional/optional_fwd.hpp>
	#define CLARA_CONFIG_OPTIONAL_TYPE boost::optional
#else
	#include <optional>
	namespace boost {
		using std::optional;
	}
	#define CLARA_CONFIG_OPTIONAL_TYPE std::optional
#endif
#include "clara.hpp" // part of Catch
namespace clara = Catch::clara;
