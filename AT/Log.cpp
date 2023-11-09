/*
 * Copyright (c) 2023 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause.
 */

#include "AT/Log.h"
#include <cstdio>

namespace AT
{

void dbgln(StringView message)
{
    const String log_line = format("{}\n"sv, message);
    printf("%s", log_line.characters());
}

} // namespace AT
