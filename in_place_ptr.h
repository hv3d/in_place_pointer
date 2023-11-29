#pragma once
#include <memory.h>
//#include "Singleton.h"

struct minimal_construct {};

template<typename T, size_t mem_size>
struct in_place_ptr_simple
{
	in_place_ptr_simple() { new(getMem())T(); }
	in_place_ptr_simple(const minimal_construct&) {}
	~in_place_ptr_simple() { clear(); }

	template<typename T2>	in_place_ptr_simple(const T2& t2) { create(t2); }

	template<typename T2>
	void	operator=(const T2& t2)
	{
		clear();
		create(t2);
	}

	T& operator*() const { return *get(); }
	T* operator->() const { return get(); }
	T* get() const { return (T*)getMem(); }
	void* getMem() const { return (void*)memory; }
protected:
	in_place_ptr_simple(const in_place_ptr_simple& h) {}
	in_place_ptr_simple(in_place_ptr_simple&& h) {}

	void	clear() { get()->~T(); }

	template<typename T2>
	void	create(const T2& t2)
	{
		static_assert(sizeof(T2) <= mem_size, "Class larger than storage");
		new(getMem())T2(t2);
	}

	char	memory[mem_size];
};

template <typename T>
struct in_place_ptr_copier_base
{
	virtual void callCopyCtor(const T& t, void* mem) const {}
	virtual void callMoveCtor(T& t, void* mem)		const {}
	virtual void selfCopy(void* mem)				const {}
};

template <typename T, typename T2>
struct in_place_ptr_copier : public in_place_ptr_copier_base<T>
{
	virtual void callCopyCtor(const T& t, void* mem)const
	{
		const T2& t2 = (const T2&)t;

		new(mem)T2(t2);
	}
	virtual void callMoveCtor(T& t, void* mem)const
	{
		T2& t2 = (T2&)t;

		new(mem)T2(t2);
	}

	virtual void selfCopy(void* mem)				const
	{
		new(mem)in_place_ptr_copier<T, T2>(*this);
	}
};

template<typename T, size_t mem_size>
struct in_place_ptr_copy_fn
{
	in_place_ptr_copy_fn() : copier(minimal_construct()) { new(getMem())T(); makeCopier<T>(); }
	~in_place_ptr_copy_fn() { clear(); }

	in_place_ptr_copy_fn(const in_place_ptr_copy_fn& h) : copier(minimal_construct())
	{
		h.copier->selfCopy(copier.getMem());
		h.copier->callCopyCtor(*h, getMem());
	}

	in_place_ptr_copy_fn(in_place_ptr_copy_fn&& h) : copier(minimal_construct())
	{
		h.copier->selfCopy(copier.getMem());
		h.copier->callMoveCtor(*h, getMem());
	}

	template<typename T2>	in_place_ptr_copy_fn(T2 t2) : copier(minimal_construct()) { create(t2); }

	template<typename T2>
	void	operator=(const T2& t2)
	{
		clear();
		create(t2);
	}

	T& operator*() const { return *get(); }
	T* operator->() const { return get(); }
protected:

	void	clear() { get()->~T(); }

	template<typename T2>
	void makeCopier()
	{
		copier = in_place_ptr_copier<T, T2>();
	}

	template<typename T2>
	void	create(T2& t2)
	{
		static_assert(sizeof(T2) <= mem_size, "Class larger than storage");
		makeCopier<T2>();
		new(getMem())T2(t2);
	}

	template<typename T2>
	void	create(const T2& t2)
	{
		static_assert(sizeof(T2) <= mem_size, "Class larger than storage");
		makeCopier<T2>();
		new(getMem())T2(t2);
	}

	T* get() const { return (T*)getMem(); }
	void* getMem() const { return (void*)memory; }

	char	memory[mem_size];
	in_place_ptr_simple<in_place_ptr_copier_base<T>, sizeof(in_place_ptr_copier_base<T>)>	copier;
};

template<size_t class_size, size_t mem_size>
void mem_size_check()
{
	static_assert(class_size <= mem_size, "Class larger than storage");
}

template <typename T, typename T2>
void copyCtorImpl(const T& t, void* mem)
{
	const T2& t2 = (const T2&)t;

	new(mem)T2(t2);
}
template <typename T, typename T2>
void moveCtorImpl(T& t, void* mem)
{
	T2& t2 = (T2&)t;

	new(mem)T2(t2);
}

template<typename T, size_t mem_size>
struct in_place_ptr_move_copy
{
	using Self = in_place_ptr_move_copy<T, mem_size>;
	in_place_ptr_move_copy() { new(getMem())T(); makeCopier<T>(); }
	~in_place_ptr_move_copy() { clear(); }

	in_place_ptr_move_copy(const Self& h)
	{
		copy_ctor_fn = h.copy_ctor_fn;
		move_ctor_fn = h.move_ctor_fn;
		copy_ctor_fn(*h, getMem());
	}

	in_place_ptr_move_copy(Self&& h)
	{
		copy_ctor_fn = h.copy_ctor_fn;
		move_ctor_fn = h.move_ctor_fn;
		move_ctor_fn(*h, getMem());
	}

	template<typename T2>	in_place_ptr_move_copy(const T2& t2) { create(t2); }
	//	template<typename T2>	in_place_ptr_move_copy(T2 &&t2) { create(t2); }

	template<typename T2>
	void	operator=(const T2& t2)
	{
		clear();
		create(t2);
	}

	T& operator*() const { return *get(); }
	T* operator->() const { return get(); }
protected:
	typedef	void (*copyCtor)(const T&, void*);
	typedef	void (*moveCtor)(T&, void*);


	void	clear() { get()->~T(); }

	template<typename T2>
	void makeCopier()
	{
		copy_ctor_fn = &copyCtorImpl<T, T2>;
		move_ctor_fn = &moveCtorImpl<T, T2>;
	}

	template<typename T2>
	void	create(T2& t2)
	{
		mem_size_check<sizeof(T2), mem_size>();
		makeCopier<T2>();
		new(getMem())T2(t2);
	}

	template<typename T2>
	void	create(const T2& t2)
	{
		static_assert(sizeof(T2) <= mem_size, "Class larger than storage");
		makeCopier<T2>();
		new(getMem())T2(t2);
	}

	T* get() const { return (T*)getMem(); }
	void* getMem() const { return (void*)memory; }

	char		memory[mem_size];
	copyCtor	copy_ctor_fn;
	moveCtor	move_ctor_fn;
};


// A holder that relies on memcopy to move construct copies
template<typename T, size_t mem_size>
struct in_place_ptr_memcpy : public in_place_ptr_simple<T, mem_size>
{
	in_place_ptr_memcpy() {}

	// Allow for dumb move - using memcopy
	in_place_ptr_memcpy(in_place_ptr_memcpy&& h) : in_place_ptr_simple<T, mem_size>(minimal_construct())
	{
		memcpy(in_place_ptr_simple<T, mem_size>::getMem(), h.getMem(), mem_size);
		// Overwrite old value!
		new (h.getMem()) T();
	}

	template<typename T2>	in_place_ptr_memcpy(const T2& t2) : in_place_ptr_simple<T, mem_size>(t2) {}
	template<typename T2>	in_place_ptr_memcpy(T2&& t2) : in_place_ptr_simple<T, mem_size>(t2) {}

	T* operator->() const { return in_place_ptr_simple<T, mem_size>::get(); }
private:
	in_place_ptr_memcpy(const in_place_ptr_memcpy& h);	// Don't implement
};

