/****************************************************************************
**
** Copyright (C) 2009 TECHNOGERMA Systems France and/or its subsidiary(-ies).
** Contact: Technogerma Systems France Information (contact@technogerma.fr)
**
** This file is part of the CAMP library.
**
** Commercial Usage
** Licensees holding valid CAMP Commercial licenses may use this file in
** accordance with the CAMP Commercial License Agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and TECHNOGERMA Systems France.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL3.txt included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at sales@technogerma.fr.
**
****************************************************************************/


#include <camp/class.hpp>
#include <camp/invalidindex.hpp>
#include <camp/invalidfunction.hpp>
#include <camp/invalidproperty.hpp>


namespace camp
{
//-------------------------------------------------------------------------------------------------
const std::string& Class::name() const
{
    return m_name;
}

//-------------------------------------------------------------------------------------------------
std::size_t Class::baseCount() const
{
    return m_bases.size();
}

//-------------------------------------------------------------------------------------------------
const Class& Class::base(std::size_t index) const
{
    if (index >= m_bases.size())
        CAMP_ERROR(InvalidIndex(index, m_bases.size()));

    return *m_bases[index].base;
}

//-------------------------------------------------------------------------------------------------
std::size_t Class::functionCount() const
{
    return m_functionsByIndex.size();
}

//-------------------------------------------------------------------------------------------------
bool Class::hasFunction(const std::string& name) const
{
    return m_functions.find(name) != m_functions.end();
}

//-------------------------------------------------------------------------------------------------
const Function& Class::function(std::size_t index) const
{
    if (index >= m_functionsByIndex.size())
        CAMP_ERROR(InvalidIndex(index, m_functionsByIndex.size()));

    return *m_functionsByIndex[index];
}

//-------------------------------------------------------------------------------------------------
const Function& Class::function(const std::string& name) const
{
    FunctionTable::const_iterator it = m_functions.find(name);
    if (it == m_functions.end())
        CAMP_ERROR(InvalidFunction(name.c_str(), *this));

    return *it->second;
}

//-------------------------------------------------------------------------------------------------
std::size_t Class::propertyCount() const
{
    return m_propertiesByIndex.size();
}

//-------------------------------------------------------------------------------------------------
bool Class::hasProperty(const std::string& name) const
{
    return m_properties.find(name) != m_properties.end();
}

//-------------------------------------------------------------------------------------------------
const Property& Class::property(std::size_t index) const
{
    if (index >= m_propertiesByIndex.size())
        CAMP_ERROR(InvalidIndex(index, m_propertiesByIndex.size()));

    return *m_propertiesByIndex[index];
}

//-------------------------------------------------------------------------------------------------
const Property& Class::property(const std::string& name) const
{
    PropertyTable::const_iterator it = m_properties.find(name);
    if (it == m_properties.end())
        CAMP_ERROR(InvalidProperty(name.c_str(), *this));

    return *it->second;
}

//-------------------------------------------------------------------------------------------------
void Class::visit(ClassVisitor& visitor) const
{
    // First visit properties
    PropertyArray::const_iterator propEnd = m_propertiesByIndex.end();
    for (PropertyArray::const_iterator it = m_propertiesByIndex.begin(); it != propEnd; ++it)
    {
        (*it)->accept(visitor);
    }

    // Then visit functions
    FunctionArray::const_iterator funcEnd = m_functionsByIndex.end();
    for (FunctionArray::const_iterator it = m_functionsByIndex.begin(); it != funcEnd; ++it)
    {
        (*it)->accept(visitor);
    }
}

//-------------------------------------------------------------------------------------------------
bool Class::applyOffset(void*& pointer, const Class& target) const
{
    // Special case for null pointers: don't apply offset to leave them null
    if (!pointer)
        return true;

    // Check target as a base class of this
    int offset = baseOffset(target);
    if (offset != -1)
    {
        pointer = static_cast<void*>(static_cast<char*>(pointer) + offset);
        return true;
    }

    // Check target as a derived class of this
    offset = target.baseOffset(*this);
    if (offset != -1)
    {
        pointer = static_cast<void*>(static_cast<char*>(pointer) - offset);
        return true;
    }

    // No match found, target is not a base class nor a derived class of this
    return false;
}

//-------------------------------------------------------------------------------------------------
bool Class::operator==(const Class& other) const
{
    return m_name == other.m_name;
}

//-------------------------------------------------------------------------------------------------
bool Class::operator!=(const Class& other) const
{
    return m_name != other.m_name;
}

//-------------------------------------------------------------------------------------------------
Class::Class(const std::string& name)
    : m_name(name)
{
}

//-------------------------------------------------------------------------------------------------
int Class::baseOffset(const Class& base) const
{
    // Check self
    if (&base == this)
        return 0;

    // Search base in the base classes
    std::vector<BaseInfo>::const_iterator end = m_bases.end();
    for (std::vector<BaseInfo>::const_iterator it = m_bases.begin(); it != end; ++it)
    {
        int offset = it->base->baseOffset(base);
        if (offset != -1)
            return offset + it->offset;
    }

    return -1;
}

} // namespace camp
