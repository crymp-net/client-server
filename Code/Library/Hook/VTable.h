/**
 * @file
 * @brief Virtual function table hook.
 */

#pragma once

bool VTableHook(void *pVTable, unsigned int index, void *pNewFunc, void **pOldFunc = nullptr);
