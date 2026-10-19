#include <Pack/RPSystem.h>

/******************************************************************************
 *
 * RPSysTagParm
 *
 ******************************************************************************/

/**
 * \~english @brief Constructor
 *
 * \~english @param pParent Parent container
 * \~english @param pTag Parameter tag
 */
RPSysTagParm::RPSysTagParm(RPSysTagParameters* pParent, TagType pTag)
    : mpTag(pTag), mpNext(NULL) {

    pParent->add(this);
}

/**
 * \~english @brief Gets the parameter's tag name
 */
TagType RPSysTagParm::getTag() {
    return mpTag;
}

/**
 * \~english @brief Accesses the next parameter
 */
RPSysTagParm* RPSysTagParm::getNext() {
    return mpNext;
}
/**
 * \~english @brief Sets the next parameter
 *
 * \~english @param pNext Next parameter
 */
void RPSysTagParm::setNext(RPSysTagParm* pNext) {
    mpNext = pNext;
}

/******************************************************************************
 *
 * RPSysPrimTagParm<int>
 *
 ******************************************************************************/

/**
 * \~english @brief Reads the parameter's value
 *
 * \~english @param rStream Input stream
 */
template <> void RPSysPrimTagParm<int>::read(EGG::Stream& rStream) {
    mValue = rStream.read_s32();
}

/**
 * \~english @brief Writes the parameter's value
 *
 * \~english @param rStream Output stream
 */
template <> void RPSysPrimTagParm<int>::write(EGG::Stream& rStream) {
    rStream.write_s32(mValue);
}

/**
 * \~english @brief Prints the parameter's value
 */
template <> void RPSysPrimTagParm<int>::dump() {}

/******************************************************************************
 *
 * RPSysStringTagParm
 *
 ******************************************************************************/

/**
 * \~english @brief Constructor
 *
 * \~english @param pParent Parent container
 * \~english @param pTag Parameter tag
 */
RPSysStringTagParm::RPSysStringTagParm(RPSysTagParameters* pParent,
                                       TagType pTag)
    : RPSysTagParm(pParent, pTag), mpValue(NULL) {}

/**
 * \~english @brief Reads the parameter's value
 *
 * \~english @param rStream Input stream
 */
void RPSysStringTagParm::read(EGG::Stream& rStream) {
    mpValue = const_cast<TagType>(rStream.readString(NULL, 0));
}

/**
 * \~english @brief Writes the parameter's value
 *
 * \~english @param rStream Output stream
 */
void RPSysStringTagParm::write(EGG::Stream& rStream) {
    rStream.writeString(const_cast<char*>(mpValue));
}

/**
 * \~english @brief Prints the parameter's value
 */
void RPSysStringTagParm::dump() {}

/******************************************************************************
 *
 * RPSysTagParameters
 *
 ******************************************************************************/

/**
 * \~english @brief Constructor
 *
 * \~english @param pName Parameter group name
 */
RPSysTagParameters::RPSysTagParameters(TagType pName)
    : mpName(pName), mpParameters(NULL) {}

/**
 * \~english @brief Appends a new child parameter to this group
 *
 * \~english @param pChild Child parameter
 */
void RPSysTagParameters::add(RPSysTagParm* pChild) {
    if (mpParameters != NULL) {
        pChild->setNext(mpParameters);
        mpParameters = pChild;
    } else {
        mpParameters = pChild;
    }
}
