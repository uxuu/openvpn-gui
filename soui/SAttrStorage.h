/**
 * @file SAttrStorage.h
 * @brief Implements an attribute storage class using a key-value map.
 * @details This file defines the SAttrStorage class, which implements the IAttrStorage interface, used to store attributes as key-value pairs.
 * @author UxGood <uxgood.org@gmail.com>
 * @date 2025-06-25
 */
#pragma once

#include <map>
#include <string>

/**
 * @class SAttrStorage
 * @brief Implements IAttrStorage using a std::map to store attributes as key-value pairs.
 *
 * This class provides methods to set and get attributes by name. Attributes are stored
 * internally in a std::map, with the attribute name as the key and the attribute value
 * as the value.
 */
class SAttrStorage: public TObjRefImpl<IAttrStorage>
{
public:
    /**
     * @brief Sets an attribute value by name.
     *
     * This method stores the given attribute name and value in the internal map.
     *
     * @param strName Attribute name.
     * @param strValue Attribute value.
     * @param bHandled Indicates if the attribute was handled (unused).
     */
    STDMETHOD_(void, OnSetAttribute)
    (THIS_ const IStringW *strName, const IStringW *strValue, BOOL bHandled) OVERRIDE
    {
        m_attrs[strName->c_str()] = strValue->c_str();
    }

    /**
     * @brief Gets an attribute value by name.
     *
     * This method retrieves the value of the specified attribute name from the internal map.
     * If the attribute exists, its value is assigned to the output parameter.
     *
     * @param strName Attribute name.
     * @param strValue Output parameter for the attribute value.
     * @return TRUE if the attribute exists, FALSE otherwise.
     */
    STDMETHOD_(BOOL, OnGetAttribute)(CTHIS_ const IStringW *strName, IStringW *strValue) SCONST OVERRIDE
    {
        auto it = m_attrs.find(strName->c_str());
        if (it != m_attrs.end())
        {
            strValue->Assign(it->second.c_str());
            return TRUE;
        }
        return FALSE;
    }
public:
    /**
     * @brief Map storing attribute name-value pairs.
     *
     * This map is used internally to store attributes, with the attribute name as the key
     * and the attribute value as the value.
     */
    std::map<std::wstring, std::wstring> m_attrs;
};
