#ifndef RP_SYSTEM_TAG_PARMS_H
#define RP_SYSTEM_TAG_PARMS_H
#include <Pack/types_pack.h>

#include <egg/core.h>

//! @addtogroup rp_system
//! @{

// Forward declarations
class RPSysTagParameters;

#if defined(NONMATCHING) || defined(COMPAT_ANY)
typedef const char* TagType;
#else
typedef char* TagType;
#endif

/**
 * \~english @brief Base class for tag parameters
 */
class RPSysTagParm {
public:
    /**
     * \~english @brief Constructor
     *
     * \~english @param pParent Parent container
     * \~english @param pTag Parameter tag
     */
    RPSysTagParm(RPSysTagParameters* pParent, TagType pTag);

    /**
     * \~english @brief Reads the parameter's value
     *
     * \~english @param rStream Input stream
     */
    virtual void read(EGG::Stream& /* rStream */) {} // at 0x8
    /**
     * \~english @brief Writes the parameter's value
     *
     * \~english @param rStream Output stream
     */
    virtual void write(EGG::Stream& /* rStream */) {} // at 0xC
    /**
     * \~english @brief Prints the parameter's value
     */
    virtual void dump() {} // at 0x10

    /**
     * \~english @brief Gets the parameter's tag name
     */
    TagType getTag();

    /**
     * \~english @brief Accesses the next parameter
     */
    RPSysTagParm* getNext();
    /**
     * \~english @brief Sets the next parameter
     *
     * \~english @param pNext Next parameter
     */
    void setNext(RPSysTagParm* pNext);

private:
    //! Parameter tag
    TagType mpTag; // at 0x4
    //! Next parameter in group
    RPSysTagParm* mpNext; // at 0x8
};

/**
 * \~english @brief Primitive-typed tag parameter
 *
 * @tparam Parameter primitive type
 */
template <typename T> class RPSysPrimTagParm : public RPSysTagParm {
public:
    /**
     * \~english @brief Constructor
     *
     * \~english @param pParent Parent container
     * \~english @param pTag Parameter tag
     */
    RPSysPrimTagParm(RPSysTagParameters* pParent, TagType pTag)
        : RPSysTagParm(pParent, pTag) {}

    /**
     * \~english @brief Reads the parameter's value
     *
     * \~english @param rStream Input stream
     */
    virtual void read(EGG::Stream& rStream) override; // at 0x8
    /**
     * \~english @brief Writes the parameter's value
     *
     * \~english @param rStream Output stream
     */
    virtual void write(EGG::Stream& rStream) override; // at 0xC
    /**
     * \~english @brief Prints the parameter's value
     */
    virtual void dump() override; // at 0x10

    /**
     * \~english @brief Sets the parameter's value
     *
     * \~english @param value New value
     */
    void set(T value) {
        mValue = value;
    }

    /**
     * \~english @brief Accesses the parameter's value
     */
    T& operator()() {
        return mValue;
    }

private:
    //! Parameter value
    T mValue; // at 0xC
};

/**
 * \~english @brief String-typed tag parameter
 */
class RPSysStringTagParm : public RPSysTagParm {
public:
    /**
     * \~english @brief Constructor
     *
     * \~english @param pParent Parent container
     * \~english @param pTag Parameter tag
     */
    RPSysStringTagParm(RPSysTagParameters* pParent, TagType pTag);

    /**
     * \~english @brief Reads the parameter's value
     *
     * \~english @param rStream Input stream
     */
    virtual void read(EGG::Stream& rStream) override; // at 0x8
    /**
     * \~english @brief Writes the parameter's value
     *
     * \~english @param rStream Output stream
     */
    virtual void write(EGG::Stream& rStream) override; // at 0xC
    /**
     * \~english @brief Prints the parameter's value
     */
    virtual void dump() override; // at 0x10

    /**
     * \~english @brief Sets the parameter's value
     *
     * \~english @param value New value
     */
    void set(TagType pValue) {
        mpValue = pValue;
    }

    /**
     * \~english @brief Accesses the parameter's value
     */
    TagType& operator()() {
        return mpValue;
    }

private:
    //! Parameter value
    TagType mpValue; // at 0xC
};

/**
 * \~english @brief Group of tag parameters
 */
class RPSysTagParameters {
public:
    /**
     * \~english @brief Constructor
     *
     * \~english @param pName Parameter group name
     */
    explicit RPSysTagParameters(TagType pName);

    /**
     * \~english @brief Appends a new child parameter to this group
     *
     * \~english @param pChild Child parameter
     */
    void add(RPSysTagParm* pChild);

private:
    //! Name of parameter group
    TagType mpName; // at 0x0
    //! Child parameters
    RPSysTagParm* mpParameters; // at 0x4
};

//! @}

#endif
