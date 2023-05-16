#pragma once
#ifndef HARPY_DELEGATE
#define HARPY_DELEGATE

#include <functional>
#include <vector>
#include <string>
#include <variant>
#include <map>
#include <any>
#include <set>

namespace harpy::utilities {

#define st ::std::

template <typename return_type, class T, typename ... Args>
using method = st function<return_type(T&, Args...)>;

template<typename return_type, typename  ... Args>
using func = st function<return_type(Args...)>;

	//TODO: Complete all delegate versions and their methods
	//TODO: Add to methods, that returns object, whose methods were invoken
template<typename return_type, typename ... Types>
class delegate_methods
{
	st vector<st any> method_array;
	st set <st string> func_names;
	st map<uint64_t, st vector<int>> hash_index; 
	int last_index = 0;

	const size_t arg_num = sizeof...(Types);

public:
	
	delegate_methods() = default;

	int size() const;

	std::set<std::string> available_classes();

	st string parse_class_names(st string);

	std::set<st string> available_parsed_classes();

	size_t get_arg_am() const;

	template <class T>
	void add_method(method<return_type, T, Types...> ptr);

	void erase();

	template <class T, typename ... Params>
	st vector<return_type> invoke_all_class(Params... params);

	template <class T, typename ... Params>
	st vector<return_type> invoke_all_class_csobj(T& obj, Params ... params);
	

	template <class T, typename ... Params>
	return_type invoke_one(int index, Params... params);


	~delegate_methods() = default;
};
	
template<typename return_type, typename  ... Args>
requires(std::invocable<func<return_type, Args...>>)
class delegate
{
	const size_t arg_num = sizeof...(Args); 

public:

	size_t get_arg_am() const { return arg_num; }

	delegate() = default;

	delegate operator+=(func<return_type, Args...>&&); //for lambda
	delegate operator+=(const func<return_type, Args...>&); //for other functions

	void add_func(func<return_type, Args...>);

	void erase();

	template<typename ... Params>
	st vector<return_type> invoke_all(Params ... params);

	template<typename ... Params>
	return_type invoke_one(int index, Params... params);


private:
	st vector<func<return_type, Args...>> func_array{};
};

template <typename return_type, typename ... Types>
int delegate_methods<return_type, Types...>::size() const
{ return method_array.size(); }

template <typename return_type, typename ... Types>
std::set<std::string> delegate_methods<return_type, Types...>::available_classes()
{ return func_names; }

template <typename return_type, typename ... Types>
std::string delegate_methods<return_type, Types...>::parse_class_names(std::string str)
{
	st string result{"Return type: "};
	for (int i = str.find('<') + 1;; i++)
	{
		if (str[i] == '_') break;
		result += str[i];
	}
	result += "\nClass: ";
	for (int i = str.find('(') + 1;; i++)
	{
		if (str[i] == '&') break;
		result += str[i];
	}
	
	if (str[str.find_last_of(')')-1] != '4' || str[str.find_last_of(')') - 1] != '2')
	{
		result += "\n with return types: ";
		for (int i = str.find(',') + 1;;i++)
		{
			if (str[i] == ')') break;
			result += str[i];
		}
	} else
	{
		result += "\n without returning something";
	}
	return result;
}

template <typename return_type, typename ... Types>
std::set<std::string> delegate_methods<return_type, Types...>::available_parsed_classes()
{

	std::set<st string> res;
	for (auto element : func_names)
	{
		res.insert(parse_class_names(element));
	}
	return res;
}

template <typename return_type, typename ... Types>
size_t delegate_methods<return_type, Types...>::get_arg_am() const
{ return arg_num; }

template <typename return_type, typename ... Types>
template <class T>
void delegate_methods<return_type, Types ...>::add_method(method<return_type, T, Types...> ptr)
{
	hash_index[typeid(T).hash_code()].push_back(last_index++);
	bool flag = false;
	for (auto i : func_names) 
	{
		if (i == typeid(T).name())
		{
			flag = true;
			break;
		}
		
	}
	if (!flag) { func_names.emplace(typeid(method<return_type, T, Types...>).name()); }
	method_array.push_back(st variant<method<return_type, T, Types...>>(ptr));
}

template <typename return_type, typename ... Types>
void delegate_methods<return_type, Types...>::erase()
{
	hash_index.erase(hash_index.begin(), hash_index.end());
	func_names.erase(func_names.begin(), func_names.end());
	method_array.erase(method_array.begin(), method_array.end());
	last_index = 0;
}

template <typename return_type, typename ... Types>
template <class T, typename ... Params>
st vector<return_type> delegate_methods<return_type, Types...>::invoke_all_class(Params... params)
{
	if (!hash_index.contains(typeid(T).hash_code())) { throw st exception("Invalid class input while invoking all methods by class"); }

	const auto vec = hash_index[typeid(T).hash_code()];
	st vector<return_type> res;
	for (const auto index : vec)
	{
		T obj;
		res.emplace_back(st get<0>(st any_cast<st variant<method<return_type, T, Types...>>>(method_array.at(index)))(obj, params...));
	}
	return res;
}

template <typename return_type, typename ... Types>
template <class T, typename ... Params>
std::vector<return_type> delegate_methods<return_type, Types...>::invoke_all_class_csobj(T& obj, Params... params)
{
	if (!hash_index.contains(typeid(T).hash_code())) { throw st exception("Invalid class input while invoking all methods by class"); }

	const auto vec = hash_index[typeid(T).hash_code()];
	st vector<return_type> res;
	for (const auto index : vec)
	{
		res.push_back(st get<0>(st any_cast<st variant<method<return_type, T, Types...>>>(method_array.at(index)))(obj, params...));
	}
	return res;
}

template <typename return_type, typename ... Types>
template <class T, typename ... Params>
return_type delegate_methods<return_type, Types...>::invoke_one(int index, Params... params)
{
	bool flag = false;
	for(auto i : hash_index)
	{
		for(auto f : i.second)
		{
			if (f == index) {
				flag = true;
				break;
			}
		}
		if (flag) break;
	}

	if (!flag) { throw st exception("Incorrect index while invoking one method"); }
	
	T obj;

	return st get<0>(st any_cast<st variant<method<return_type, T, Types...>>>(method_array.at(index)))(obj, params...);
}
	

template <typename return_type, typename ... Args> requires (std::invocable<func<return_type, Args...>>)
delegate<return_type, Args...> delegate<return_type, Args...>::operator+=(func<return_type, Args...>&& fun)
{
	func_array.emplace_back(fun);
	return *this;
}

template <typename return_type, typename ... Args> requires (std::invocable<func<return_type, Args...>>)
delegate<return_type, Args...> delegate<return_type, Args...>::operator+=(const func<return_type, Args...>& fun)
{
	func_array.push_back(fun);
	return *this;
}

template <typename return_type, typename ... Args> requires (std::invocable<func<return_type, Args...>>)
void delegate<return_type, Args...>::add_func(func<return_type, Args...> fun)
{
	func_array.emplace_back(fun);
}

template <typename return_type, typename ... Args> requires (std::invocable<func<return_type, Args...>>)
template <typename ... Params>
std::vector<return_type> delegate<return_type, Args...>::invoke_all(Params... params)
{
	std::vector<return_type> result;
	for (auto element : func_array)
	{
		result.push_back(element(params...));
	}
	return result;
}

template<typename ... Types>
class delegate_methods <void, Types...>
{
	st vector<st any> method_array;

	std::set<st string> func_names;

	st map<uint64_t, st vector<int>> hash_index;
	int last_index = 0;

	const size_t arg_num = sizeof...(Types);

public:

	delegate_methods() = default;

	int size() { return method_array.size(); }

	std::set<st string> available_classes() { return func_names; }

	st string parse_class_names(st string);

	std::set<st string> available_parsed_classes() {

		std::set<st string> res;
		for (auto element : func_names)
		{
			res.insert(parse_class_names(element));
		}
		return res;
	}

	size_t get_arg_am() const { return arg_num; }

	template <class T>
	void add_method(method<void, T, Types...> ptr);

	void erase();

	template <class T>
	void erase(method<void, T, Types...> meth, int index)
	{
		if (index < -1) throw st exception("Invalid index while erasing one method");
		if (!hash_index.contains(typeid(meth).hash_code()))
		{
			throw st exception("Invalid method while trying to erase it");
		}
		if (index == -1)
		{
			auto vec = hash_index[typeid(meth).hash_code()];
			for (int i = 0; i < vec.size(); i++)
			{
				method_array.erase(method_array.begin() + vec[i]);
			}
			return;
		}
		auto vec = hash_index[typeid(meth).hash_code()];
		method_array.erase(method_array.begin() + vec[index]);
	}

	template <class T>
	void invoke_all_class(Types... params);

	template <class T>
	void invoke_all_class_csobj(T& obj, Types ... params);


	template <class T, typename ... Params>
	void invoke_one(int index, Params... params);


	~delegate_methods() = default;
};

template <typename ... Types>
std::string delegate_methods<void, Types...>::parse_class_names(std::string str)
{
	st string result{ "Return type: " };
	for (int i = str.find('<') + 1;; i++)
	{
		if (str[i] == '_') break;
		result += str[i];
	}
	result += "\nClass: ";
	for (int i = str.find('(') + 1;; i++)
	{
		if (str[i] == '&') break;
		result += str[i];
	}

	if (str[str.find_last_of(')') - 1] != '4' || str[str.find_last_of(')') - 1] != '2')
	{
		result += "\n with return types: ";
		for (int i = str.find(',') + 1;; i++)
		{
			if (str[i] == ')') break;
			result += str[i];
		}
	}
	else
	{
		result += "\n without returning something";
	}
	return result;
}

template <typename ... Types>
template <class T>
void delegate_methods<void, Types ...>::add_method(method<void, T, Types...> ptr)
{
	hash_index[typeid(T).hash_code()].push_back(last_index++);
	bool flag = false;
	for (auto i : func_names)
	{
		if (i == typeid(T).name())
		{
			flag = true;
			break;
		}

	}
	if (!flag) { func_names.emplace(typeid(method<void, T, Types...>).name()); }
	method_array.push_back(st variant<method<void, T, Types...>>(ptr));
}

template <typename ... Types>
void delegate_methods<void, Types...>::erase()
{
	hash_index.erase(hash_index.begin(), hash_index.end());
	func_names.erase(func_names.begin(), func_names.end());
	method_array.erase(method_array.begin(), method_array.end());
	last_index = 0;
}

template <typename ... Types>
template <class T>
void delegate_methods<void, Types...>::invoke_all_class(Types... params)
{
	if (!hash_index.contains(typeid(T).hash_code())) { throw st exception("Invalid class input while invoking all methods by class"); }

	const auto vec = hash_index[typeid(T).hash_code()];
	for (const auto index : vec)
	{
		T obj;
		st get<0>(st any_cast<st variant<method<void, T, Types...>>>(method_array.at(index)))(obj, params...);
	}
}

template <typename ... Types>
template <class T>
void delegate_methods<void, Types...>::invoke_all_class_csobj(T& obj, Types... params)
{
	if (!hash_index.contains(typeid(T).hash_code())) { throw st exception("Invalid class input while invoking all methods by class"); }

	const auto vec = hash_index[typeid(T).hash_code()];
	for (const auto index : vec)
	{
		st get<0>(st any_cast<st variant<method<void, T, Types...>>>(method_array.at(index)))(obj, params...);
	}
}

template <typename ... Types>
template <class T, typename ... Params>
void delegate_methods<void, Types...>::invoke_one(int index, Params... params)
{
	bool flag = false;
	for (auto i : hash_index)
	{
		for (auto f : i.second)
		{
			if (f == index) {
				flag = true;
				break;
			}
		}
		if (flag) break;
	}

	if (!flag) { throw st exception("Incorrect index while invoking one method"); }

	T obj;

	st get<0>(st any_cast<st variant<method<void, T, Types...>>>(method_array.at(index)))(obj, params...);
}

template<typename  ... Args >
class delegate <void, Args...>
{
	const size_t arg_num = sizeof...(Args);

public:

	size_t get_arg_am() const { return arg_num; }

	delegate() = default;

	delegate operator+=(func<void, Args...>&& fun)
	{
		func_array.emplace_back(fun);
		return *this;
	} //for lambda
	delegate operator+=(func<void, Args...>& fun)
	{
		func_array.push_back(fun);
		return *this;
	} //for other functions

	void add_function(func<void, Args...>& fun)
	{
		func_array.push_back(fun);
	}

	void add_function(func<void, Args...>&& fun)
	{
		func_array.emplace_back(fun);
	}

	void erase()
	{
		func_array.erase(func_array.begin(), func_array.end());
	}

	delegate& operator=(func<void, Args...>&& fun)
	{
		erase();
		add_function(fun);
		return *this;
	}

	delegate& operator=(func<void, Args...>& fun)
	{
		erase();
		add_function(fun);
		return *this;
	}

	void invoke_all(Args ... params)
	{
		for (auto element : func_array)
		{
			element(params...);
		}
	}

	void operator()(Args... params)
	{
		invoke_all(params...);
	}

	void invoke_one(int index, Args... params)
	{
		if (index < 0 || index > func_array.size()) throw st exception("Incorrect index while invoking void delegate");
		func_array[index](params...);
	}


private:
	st vector<func<void, Args...>> func_array{};
};


#undef st
}
#endif