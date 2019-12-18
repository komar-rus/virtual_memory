#ifndef VIRTUAL_MEMORY_H
#define VIRTUAL_MEMORY_H

#include <memory>
#include <vector>
#include <cstring>
#include <assert.h>
#include <initializer_list>

template <class T = uint8_t>
class virtual_memory_t
{
    struct entry_t
    {
        entry_t(ptrdiff_t s, ptrdiff_t e, size_t l, T *p) :
            start(s),
            stop(e),
            size(l),
            pData(p) {};

        ptrdiff_t start;
        ptrdiff_t stop;
        size_t size;
        T *pData;
    };

public:
    virtual_memory_t();
    virtual_memory_t(const virtual_memory_t<T> &copy);
    virtual_memory_t(virtual_memory_t<T> *p);
    virtual_memory_t(virtual_memory_t<T> &&copy);
    virtual_memory_t(const std::initializer_list<std::pair<T*, size_t>> &list);

    ~virtual_memory_t();

    virtual_memory_t<T> operator = (virtual_memory_t<T> *p);
    virtual_memory_t<T> &operator = (const virtual_memory_t<T> &vm);
    virtual_memory_t<T> &operator = (virtual_memory_t<T> &&vm);

    T &operator [] (size_t n) const;
    virtual_memory_t<T> &operator += (size_t n);
    virtual_memory_t<T> &operator -= (size_t n);
    virtual_memory_t<T> &operator ++ ();
    virtual_memory_t<T> operator ++ (int);
    virtual_memory_t<T> &operator -- (void);
    virtual_memory_t<T> operator -- (int);
    virtual_memory_t<T> operator + (size_t n) const;
    virtual_memory_t<T> operator - (size_t n) const;

    T &operator * (void) const;

    void push_back(T *pData, size_t size);
    void push_back(virtual_memory_t<T> &pData, size_t size);

    inline size_t size() const { return m_size; };
    inline size_t length() const { return m_size - m_pos; };
    inline void clear(void);

    size_t copyFrom(T *pData, size_t size, size_t offset = 0);
    void memset(T value, size_t size, size_t offset = 0);

private:
    std::shared_ptr<std::vector<entry_t>> m_listEntry;
    size_t m_size;

    ptrdiff_t m_nEntry;
    ptrdiff_t m_pos;
    bool m_init;
    T *m_pData;
};

typedef virtual_memory_t<> virtual_memory;

template <class T>
virtual_memory_t<T>::virtual_memory_t():
    m_size(0),
    m_nEntry(0),
    m_pos(0),
    m_init(false),
    m_pData(nullptr)
{
}

template <class T>
virtual_memory_t<T>::virtual_memory_t(const virtual_memory_t<T> &copy)
{
    m_listEntry = copy.m_listEntry;
    m_size = copy.m_size;
    m_init = copy.m_init;
    m_nEntry = copy.m_nEntry;
    m_pos = copy.m_pos;
    m_pData = copy.m_pData;
}

template <class T>
virtual_memory_t<T>::virtual_memory_t(virtual_memory_t<T> *p)
{
    *this = *p;
}

template <class T>
virtual_memory_t<T>::virtual_memory_t(virtual_memory_t<T> &&copy)
{
    std::swap(copy.m_listEntry, m_listEntry);
    std::swap(copy.m_size, m_size);
    std::swap(copy.m_init, m_init);
    std::swap(copy.m_nEntry, m_nEntry);
    std::swap(copy.m_pos, m_pos);
    std::swap(copy.m_pData, m_pData);
}

template <class T>
virtual_memory_t<T>::virtual_memory_t(const std::initializer_list<std::pair<T*, size_t>> &list):
    virtual_memory_t()
{
    for (auto &pair : list)
        push_back(pair.first, pair.second);
}

template <class T>
virtual_memory_t<T>::~virtual_memory_t()
{
}

template <class T>
void virtual_memory_t<T>::push_back(T *pData, size_t size)
{
    if (nullptr == m_listEntry)
        m_listEntry = std::make_shared<std::vector<entry_t>>();

    m_listEntry->emplace_back(m_size, m_size + size, size, pData);
    m_size += size;

    if (false == m_init)
    {
        m_nEntry = 0;
        m_init = true;
        m_pData = pData;
    }
}

template <class T>
void virtual_memory_t<T>::push_back(virtual_memory_t<T> &vm, size_t size)
{
    if (nullptr == m_listEntry)
        m_listEntry = std::make_shared<std::vector<entry_t>>();

    ptrdiff_t pos = vm.m_pos;
    size_t n = vm.m_nEntry;
    while ((size > 0) && (n < vm.m_listEntry->size()))
    {
        // get entry from list
        const entry_t &entry = vm.m_listEntry->at(n);

        size_t length = std::min(size, (size_t)(entry.stop - pos));
        add(entry.pData + (pos - entry.start), length);

        size -= length;
        pos += length;
        ++n;
    }

    assert(size == 0);
}

template <class T>
void virtual_memory_t<T>::clear(void)
{
    m_nEntry = 0;
    m_init = false;
    m_size = 0;
    m_pos = 0;
    m_pData = nullptr;

    if (nullptr != m_listEntry)
    {
        if (1 < m_listEntry.use_count())
        {
            m_listEntry = std::make_shared<std::vector<entry_t>>();
            m_listEntry->reserve(500);
        }
        else
            m_listEntry->clear();
    }
}

template <class T>
virtual_memory_t<T> virtual_memory_t<T>::operator = (virtual_memory_t<T> *p)
{
    return *(virtual_memory_t<T> *)p;
}

template <class T>
virtual_memory_t<T> &virtual_memory_t<T>::operator = (const virtual_memory_t<T> &vm)
{
    m_listEntry = vm.m_listEntry;
    m_size = vm.m_size;
    m_init = vm.m_init;
    m_nEntry = vm.m_nEntry;
    m_pos = vm.m_pos;
    m_pData = vm.m_pData;

    return *this;
}

template <class T>
virtual_memory_t<T> &virtual_memory_t<T>::operator = (virtual_memory_t<T> &&vm)
{
    std::swap(vm.m_listEntry, m_listEntry);
    std::swap(vm.m_size, m_size);
    std::swap(vm.m_init, m_init);
    std::swap(vm.m_nEntry, m_nEntry);
    std::swap(vm.m_pos, m_pos);
    std::swap(vm.m_pData, m_pData);
}

template <class T>
virtual_memory_t<T> & virtual_memory_t<T>::operator += (size_t n)
{
    m_pos += n;

    if (m_pos >= (ptrdiff_t)m_size)
    {
        m_nEntry = m_listEntry->size() - 1;
        return *this;
    }

    while (m_pos >= m_listEntry->at(m_nEntry).stop)
        m_nEntry++;

    const entry_t &entry = m_listEntry->at(m_nEntry);
    ptrdiff_t offset = m_pos - entry.start;
    m_pData = entry.pData + offset;

    return *this;
}

template <class T>
virtual_memory_t<T> & virtual_memory_t<T>::operator -= (size_t n)
{
    m_pos -= n;

    if (0 > m_pos)
    {
        m_nEntry = 0;
        return *this;
    }

    while (m_pos < m_listEntry->at(m_nEntry).start)
        m_nEntry--;

    const entry_t &entry = m_listEntry->at(m_nEntry);
    ptrdiff_t offset = m_pos - entry.start;
    m_pData = entry.pData + offset;

    return *this;
}

template <class T>
virtual_memory_t<T> &virtual_memory_t<T>::operator ++ (void)
{
    (*this) += 1;
    return *this;
}

template <class T>
virtual_memory_t<T> &virtual_memory_t<T>::operator -- (void)
{
    (*this) -= 1;
    return *this;
}

template <class T>
virtual_memory_t<T> virtual_memory_t<T>::operator ++ (int)
{
    virtual_memory_t<T> oldMem = *this;
    (*this) += 1;
    return oldMem;
}

template <class T>
virtual_memory_t<T> virtual_memory_t<T>::operator -- (int)
{
    virtual_memory_t<T> oldMem = *this;
    (*this) -= 1;
    return oldMem;
}

template <class T>
virtual_memory_t<T> virtual_memory_t<T>::operator - (size_t n) const
{
    virtual_memory_t<T> vm = (*this);
    vm -= n;
    return vm;
}

template <class T>
virtual_memory_t<T> virtual_memory_t<T>::operator + (size_t n) const
{
    virtual_memory_t<T> vm = (*this);
    vm += n;
    return vm;
}

template <class T>
T &virtual_memory_t<T>::operator * (void) const
{
    return m_pData[0];
}

template <class T>
T &virtual_memory_t<T>::operator [] (size_t n) const
{
    ptrdiff_t pos = m_pos + n;

    auto nEntry = m_nEntry;
    while (pos >= m_listEntry->at(nEntry).stop)
            nEntry++;

    entry_t &entry = m_listEntry->at(nEntry);
    ptrdiff_t offset = pos - entry.start;

    return entry.pData[offset];
}

template <class T>
size_t virtual_memory_t<T>::copyFrom(T *pData, size_t size, size_t offset)
{
    ptrdiff_t pos = m_pos + offset;
            size_t ret = size;

    size_t nEntry = m_nEntry;
    while (pos >= m_listEntry->at(nEntry).stop)
        nEntry++;

    while ((size > 0) && (nEntry < m_listEntry->size()))
    {
        const entry_t &entry = m_listEntry->at(nEntry);
        size_t entrySize = (size_t)(entry.stop - pos);
        size_t length = (std::min)(size, entrySize);

        uint8_t *pStartData = entry.pData + (pos - entry.start);
        ::memcpy(pStartData, pData, length);

        pData += length;
        size -= length;
        pos += length;
        ++nEntry;
    }

    return ret - size;
}

template <class T>
void  virtual_memory_t<T>::memset(T value, size_t size, size_t offset)
{
    ptrdiff_t pos = m_pos + offset;

    size_t nEntry = m_nEntry;
    while (pos >= m_listEntry->at(nEntry).stop)
        nEntry++;

    while ((size > 0) && (nEntry < m_listEntry->size()))
    {
        const entry_t &entry = m_listEntry->at(nEntry);
        size_t entrySize = (size_t)(entry.stop - pos);
        size_t length = (std::min)(size, entrySize);

        uint8_t *pStartData = entry.pData + (pos - entry.start);
        ::memset(pStartData, value, length);

        size -= length;
        pos += length;
        ++nEntry;
    }
}

#endif // VIRTUAL_MEMORY_H
