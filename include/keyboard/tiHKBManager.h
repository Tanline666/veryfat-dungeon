#ifndef KEY_TI_HKB_MANAGER
#define KEY_TI_HKB_MANAGER

#include <types.h>

namespace textinput {
namespace input {
class KBDListener {
public:
    /**
     * \~english @brief Constructor
     * \~japanese @brief コンストラクタ
     */
    KBDListener() : mpPrevListener(NULL), mpNextListener(NULL) {}
    virtual ~KBDListener() {}

private:
    KBDListener* mpPrevListener;
    KBDListener* mpNextListener;
};
} // namespace input
} // namespace textinput

#endif
