/**
 * @file SAttrStorageFactory.h
 * @brief Defines a template factory class for creating attribute storage objects.
 * @details This file contains the definition of the SAttrStorageFactory class, which implements the IAttrStorageFactory interface
 * @author UxGood <uxgood.org@gmail.com>
 * @date 2025-06-25
 */

#pragma once

/**
 * @class SAttrStorageFactory
 * @brief Template factory class for creating attribute storage objects.
 *
 * This class implements the IAttrStorageFactory interface and provides a method
 * to create instances of attribute storage objects. The template parameter T
 * should be a class implementing IAttrStorage.
 *
 * @tparam T The type of attribute storage to create.
 */

template <typename T>
class SAttrStorageFactory:public TObjRefImpl<IAttrStorageFactory>
{
public:
    /**
     * @brief Creates an instance of attribute storage.
     *
     * This method creates a new instance of the attribute storage class specified
     * by the template parameter T and returns it via the output parameter.
     *
     * @param owner The owner window (unused in this implementation).
     * @param ppAttrStorage Output pointer to the created attribute storage object.
     * @return HRESULT indicating success (S_OK) or failure.
     */
    STDMETHOD_(HRESULT, CreateAttrStorage)
    (CTHIS_ IWindow * owner, IAttrStorage * *ppAttrStorage) SCONST OVERRIDE
    {
        *ppAttrStorage = new T();
        return S_OK;
    }
};
