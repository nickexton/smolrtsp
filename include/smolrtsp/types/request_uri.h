/**
 * @file
 * An RTSP request URI.
 */

#pragma once

#include <smolrtsp/types/error.h>

#include <stdbool.h>

#include <slice99.h>

/**
 * An RTSP request URI.
 */
typedef CharSlice99 SmolRTSP_RequestUri;

/**
 * Parses @p data to @p self.
 *
 * @pre `self != NULL`
 */
SmolRTSP_ParseResult
SmolRTSP_RequestUri_parse(SmolRTSP_RequestUri *restrict self, CharSlice99 input);

/**
 * Tests @p lhs and @p rhs for equality.
 */
bool SmolRTSP_RequestUri_eq(SmolRTSP_RequestUri lhs, SmolRTSP_RequestUri rhs);
