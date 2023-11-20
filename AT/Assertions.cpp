/*
 * Copyright (c) 2023 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause.
 */

#include "AT/Assertions.h"
#include "AT/Log.h"

namespace AT
{

void verification_failed(
    const char* expression, const char* file, const char* function, u32 line
)
{
    dbgln("VERIFICATION FAILED: {}"sv, StringView::from_null_terminated_utf8(expression));
    dbgln("    IN FILE:         {}"sv, StringView::from_null_terminated_utf8(file));
    dbgln("    IN FUNCTION:     {}"sv, StringView::from_null_terminated_utf8(function));
    dbgln("    ON LINE:         {}"sv, line);

    // TODO: Call the platform API in order to open a popup window that will display the error
    // message. This feature requires extensive access to the platform layer.
}

} // namespace AT
