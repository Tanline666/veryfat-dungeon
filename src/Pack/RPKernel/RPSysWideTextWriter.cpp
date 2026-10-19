#include <Pack/RPKernel.h>

/**
 * \~english @brief Work buffer for string formatting
 */
wchar_t* RPSysWideTextWriter::spTextBuffer = NULL;

/**
 * \~english @brief Creates the text buffer
 */
void RPSysWideTextWriter::Initialize() {
    spTextBuffer = new wchar_t[TEXT_BUFFER_SIZE];
}
