#pragma once

#define UE_LOG_IF(Cond, CategoryName, Verbosity, Format, ...) \
{ \
	CA_CONSTANT_IF(Cond && (ELogVerbosity::Verbosity & ELogVerbosity::VerbosityMask) <= ELogVerbosity::COMPILED_IN_MINIMUM_VERBOSITY && (ELogVerbosity::Warning & ELogVerbosity::VerbosityMask) <= FLogCategory##CategoryName::CompileTimeVerbosity) \
	{ \
		UE_INTERNAL_LOG_IMPL(CategoryName, Verbosity, Format, ##__VA_ARGS__); \
	} \
}