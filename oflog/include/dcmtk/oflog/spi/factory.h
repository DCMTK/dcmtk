// -*- C++ -*-
// Module:  Log4CPLUS
// File:    factory.h
// Created: 2/2002
// Author:  Tad E. Smith
//
//
// Copyright 2002-2010 Tad E. Smith
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

/** @file */

#ifndef DCMTK_LOG4CPLUS_SPI_FACTORY_HEADER_
#define DCMTK_LOG4CPLUS_SPI_FACTORY_HEADER_

#include "dcmtk/oflog/config.h"

#if defined (DCMTK_LOG4CPLUS_HAVE_PRAGMA_ONCE)
#pragma once
#endif

#include "dcmtk/oflog/appender.h"
#include "dcmtk/oflog/layout.h"
#include "dcmtk/oflog/tstring.h"
#include "dcmtk/oflog/spi/filter.h"
#include "dcmtk/oflog/spi/objreg.h"
#include <memory>
#include "dcmtk/ofstd/ofvector.h"
#include <locale>


namespace dcmtk {
namespace log4cplus {
    namespace spi {

        /**
         * This is the base class for all factories.
         */
        class DCMTK_LOG4CPLUS_EXPORT BaseFactory {
        public:
            virtual ~BaseFactory() = 0;

            /**
             * Returns the typename of the objects this factory creates.
             */
            virtual log4cplus::tstring const & getTypeName() const = 0;
        };


        /**
         * This abstract class defines the "Factory" interface to create "Appender"
         * objects.
         */
        class DCMTK_LOG4CPLUS_EXPORT AppenderFactory : public BaseFactory {
        public:
            typedef Appender ProductType;
            typedef SharedAppenderPtr ProductPtr;

            AppenderFactory();
            virtual ~AppenderFactory() = 0;

            /**
             * Create an "Appender" object.
             */
            virtual SharedAppenderPtr createObject(const log4cplus::helpers::Properties& props) = 0;
        };



        /**
         * This abstract class defines the "Factory" interface to create "Layout"
         * objects.
         */
        class DCMTK_LOG4CPLUS_EXPORT LayoutFactory : public BaseFactory {
        public:
            typedef Layout ProductType;
            typedef OFauto_ptr<Layout> ProductPtr;

            LayoutFactory();
            virtual ~LayoutFactory() = 0;

            /**
             * Create a "Layout" object.
             */
            virtual OFauto_ptr<Layout> createObject(const log4cplus::helpers::Properties& props) = 0;
        };



        /**
         * This abstract class defines the "Factory" interface to create "Appender"
         * objects.
         */
        class DCMTK_LOG4CPLUS_EXPORT FilterFactory : public BaseFactory {
        public:
            typedef Filter ProductType;
            typedef FilterPtr ProductPtr;

            FilterFactory();
            virtual ~FilterFactory() = 0;

            /**
             * Create a "Filter" object.
             */
            virtual FilterPtr createObject(const log4cplus::helpers::Properties& props) = 0;
        };


        /**
         * This abstract class defines the "Factory" interface to
         * create STD_NAMESPACE locale instances.
         */
        class DCMTK_LOG4CPLUS_EXPORT LocaleFactory
            : public BaseFactory
        {
        public:
            typedef STD_NAMESPACE locale ProductType;
            typedef STD_NAMESPACE locale ProductPtr;

            LocaleFactory();
            virtual ~LocaleFactory() = 0;

            //! \returns STD_NAMESPACE locale instance
            virtual ProductPtr createObject (
                const log4cplus::helpers::Properties & props) = 0;
        };


        /**
         * This template class is used as a "Factory Registry".  Objects are
         * "entered" into the registry with a "name" using the 
         * <code>put()</code> method.  (The registry then owns the object.)  
         * These object can then be retrieved using the <code>get()</code> 
         * method.
         * 
         * <b>Note:</b>  This class is Thread-safe.
         */
        template<class T>
        class DCMTK_LOG4CPLUS_EXPORT FactoryRegistry : ObjectRegistryBase {
        public:
            typedef T product_type;

            virtual ~FactoryRegistry() {
                clear();
            }

          // public methods
            /**
             * Used to enter an object into the registry.  (The registry now
             *  owns <code>object</code>.)
             */
            bool put(OFauto_ptr<T> object) {
                 bool putValResult = putVal(object->getTypeName(), object.get());
                 object.release();
                 return putValResult; 
            }

            /**
             * Used to retrieve an object from the registry.  (The registry
             * owns the returned pointer.)
             */
            T* get(const log4cplus::tstring& name) const {
                return OFstatic_cast(T*, getVal(name));
            }

        protected:
            virtual void deleteObject(void *object) const {
                delete OFstatic_cast(T*, object);
            }
        };


        typedef FactoryRegistry<AppenderFactory> AppenderFactoryRegistry;
        typedef FactoryRegistry<LayoutFactory> LayoutFactoryRegistry;
        typedef FactoryRegistry<FilterFactory> FilterFactoryRegistry;
        typedef FactoryRegistry<LocaleFactory> LocaleFactoryRegistry;


        /**
         * Returns the "singleton" <code>AppenderFactoryRegistry</code>.
         */
        DCMTK_LOG4CPLUS_EXPORT AppenderFactoryRegistry& getAppenderFactoryRegistry();

        /**
         * Returns the "singleton" <code>LayoutFactoryRegistry</code>.
         */
        DCMTK_LOG4CPLUS_EXPORT LayoutFactoryRegistry& getLayoutFactoryRegistry();

        /**
         * Returns the "singleton" <code>FilterFactoryRegistry</code>.
         */
        DCMTK_LOG4CPLUS_EXPORT FilterFactoryRegistry& getFilterFactoryRegistry();

        /**
         * Returns the "singleton" <code>LocaleFactoryRegistry</code>.
         */
        DCMTK_LOG4CPLUS_EXPORT LocaleFactoryRegistry& getLocaleFactoryRegistry();


        template <typename ProductFactoryBase>
        class LocalFactoryBase
            : public ProductFactoryBase
        {
        public:
            LocalFactoryBase (tchar const * n)
                : name (n)
            { }

            virtual log4cplus::tstring const & getTypeName() const
            {
                return name;
            }

        private:
            log4cplus::tstring name;
        };


        template <typename LocalProduct, typename ProductFactoryBase>
        class FactoryTempl
            : public LocalFactoryBase<ProductFactoryBase>
        {
        public:
            typedef typename ProductFactoryBase::ProductPtr ProductPtr;

            FactoryTempl (tchar const * n)
                : LocalFactoryBase<ProductFactoryBase> (n)
            { }

            virtual ProductPtr createObject (helpers::Properties const & props)
            {
                return ProductPtr (new LocalProduct (props));
            }
        };


        #define DCMTK_LOG4CPLUS_REG_PRODUCT(reg, productprefix, productname, productns, productfact) \
        reg.put (																               \
            OFauto_ptr<productfact> (                                                       \
                    new log4cplus::spi::FactoryTempl<productns productname, productfact> (     \
                    DCMTK_LOG4CPLUS_TEXT(productprefix)                                              \
                    DCMTK_LOG4CPLUS_TEXT(#productname))))

        #define DCMTK_LOG4CPLUS_REG_APPENDER(reg, appendername)                             \
        DCMTK_LOG4CPLUS_REG_PRODUCT (reg, "log4cplus::", appendername, log4cplus::,         \
            log4cplus::spi::AppenderFactory) 

        #define DCMTK_LOG4CPLUS_REG_LAYOUT(reg, layoutname)                                 \
        DCMTK_LOG4CPLUS_REG_PRODUCT (reg, "log4cplus::", layoutname, log4cplus::,           \
            log4cplus::spi::LayoutFactory)

        #define DCMTK_LOG4CPLUS_REG_FILTER(reg, filtername)                                 \
        DCMTK_LOG4CPLUS_REG_PRODUCT (reg, "log4cplus::spi::", filtername, log4cplus::spi::, \
            log4cplus::spi::FilterFactory)

        #define DCMTK_LOG4CPLUS_REG_LOCALE(reg, name, factory)            \
            reg.put (OFauto_ptr<log4cplus::spi::LocaleFactory> ( \
                    new factory (name)))
    } // namespace spi
}
} // end namespace dcmtk


#endif // DCMTK_LOG4CPLUS_SPI_FACTORY_HEADER_
