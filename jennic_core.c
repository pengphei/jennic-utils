#include "jennic_core.h"

static stjn_wrapper_t _g_wrapper;

void jennic_wrapper_init(pstjn_wrapper_t pwrapper)
{
    _g_wrapper = *pwrapper;
}
