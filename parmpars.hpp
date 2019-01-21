/*
 * MIT License
 * 
 * Copyright (c) 2018 Alexander Kernozhitsky <sh200105@mail.ru>
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef __PARM_PARS_HPP_INCLUDED__
#define __PARM_PARS_HPP_INCLUDED__

#define PARM_PARS_VERSION "0.4.0~alpha"

// defines:
//   PARMPARS_EXIT_ON_WARNING
//   PARMPARS_USE_REGEX

// TODO : cache parsed variable values (?)
// TODO : add inline (?)
// TODO : color errors/warnings (?)

#ifdef PARMPARS_USE_REGEX
	#include <regex>
#endif

#include <sstream>
#include <type_traits>
#include <iostream>
#include <cstdlib>
#include <cctype>
#include <map>

#ifdef _TESTLIB_H_
	#define USE_TESTLIB
#endif

namespace ParmParsInternal {

const long double eps = 1e-12;

namespace Alerts {
	enum class Severity {
		Error,
		Warning,
		Note
	};
	
	const char *names[] = {
		"Error",
		"Warning",
		"Note"
	};
	
	bool hadError = false;
	
	void processAlert(Severity severity) {
		bool mustExit = severity == Severity::Error;
		#ifdef PARMPARS_EXIT_ON_WARNING
			mustExit |= severity == Severity::Warning;
		#endif
		if (mustExit) {
			hadError = true;
			exit(1);
		}
	}
	
	void showAlert(Severity severity, const std::string &text) {
		std::cerr << names[static_cast<int>(severity)] << ": " << text << std::endl;
	}
	
	void raiseAlert(Severity severity, const std::string &text) {
		showAlert(severity, text);
		processAlert(severity);
	}
	
	void error(const std::string &text) {
		raiseAlert(Severity::Error, text);
	}
	
	void warning(const std::string &text) {
		raiseAlert(Severity::Warning, text);
	}
	
	void note(const std::string &text) {
		raiseAlert(Severity::Note, text);
	}
}


class StringBuilder {
private:
	std::ostringstream stream;
public:
	template<typename T>
	StringBuilder &operator<<(const T &value) {
		stream << value;
		return *this;
	}
	
	operator std::string() const {
		return stream.str();
	}
};


template<typename T>
struct Range {
	T left, right;
};

template<typename T>
typename std::enable_if<std::is_floating_point<T>::value, bool>::type
isRangeValid(const Range<T> &res) {
	return res.right - res.left > -eps;
}

template<typename T>
typename std::enable_if<std::is_integral<T>::value, bool>::type
isRangeValid(const Range<T> &res) {
	return res.left <= res.right;
}

template<typename T>
Range<T> range(const T &left, const T &right) {
	Range<T> res {left, right};
	if (!isRangeValid(res)) {
		Alerts::error(StringBuilder()
			<< "Range [" << left << "; " << right << "] is invalid!"
		);
	}
	return res;
}

template<typename RangeType, typename ValueType, typename Enable = void>
struct InRange {};

template<typename RangeType, typename ValueType>
bool inRange(const Range<RangeType> &range, const ValueType &value) {
	return InRange<RangeType, ValueType>::doIt(range, value);
}

template<typename T>
struct InRange<T, T, typename std::enable_if<std::is_integral<T>::value>::type> {
	static bool doIt(const Range<T> &range, const T &value) {
		return range.left <= value && value <= range.right;
	}
};

template<typename T>
struct InRange<T, T, typename std::enable_if<std::is_floating_point<T>::value>::type> {
	static bool doIt(const Range<T> &range, const T &value) {
		return
			abs(range.left - value) < eps ||
			abs(range.right - value) < eps ||
			(range.left < value && value < range.right)
		;
	}
};


template<typename T, typename Enable, typename...> struct DataValidate {};

template<typename T, typename... ValidateMeans>
void dataValidate(const char *varName, const T &value, ValidateMeans... means) {
	DataValidate<T, void, ValidateMeans...>::doIt(varName, value, means...);
}

template<typename T>
struct DataValidate<T, void> {
	static void doIt(const char *, const T &) {}
};

template<typename T, typename R>
struct DataValidate<T, void, Range<R>> {
	static void doIt(const char *varName, const T &value, const Range<R> &range) {
		if (!inRange(range, value)) {
			Alerts::error(StringBuilder()
				<< varName << " = " << value << " is not in the range "
				<< "[" << range.left << "; " << range.right << "]"
			);
		}
	}
};

#if defined(PARMPARS_USE_REGEX) || defined(USE_TESTLIB)
template<typename T>
struct DataValidate<std::string, typename std::enable_if<std::is_convertible<T, std::string>::value>::type, T> {
	static void doIt(const char *varName, const std::string &value, const std::string &re) {
		#ifdef PARMPARS_USE_REGEX
		if (!std::regex_match(value, std::regex(re))) {
		#else
		if (pattern(re).matches(value)) {
		#endif
			Alerts::error(StringBuilder()
				<< varName << " = \"" << value << "\" doesn\'t match regex"
				<< "\"" << re << "\""
			);
		}
	}
};
#endif


template<typename T, typename = void> struct ReadFromString {};

template<typename T>
struct ReadFromString<T, typename std::enable_if<std::is_arithmetic<T>::value>::type> {
	static bool doIt(T &value, const std::string &str) {
		if (str.empty() || str[0] <= ' ') {
			return false;
		}
		std::istringstream is(str);
		if (!(is >> value)) {
			return false;
		}
		if (!is.eof()) {
			return false;
		}
		return true;
	}
};

template<>
struct ReadFromString<std::string> {
	static bool doIt(std::string &value, const std::string &str) {
		value = str;
		return true;
	}
};

template<>
struct ReadFromString<char> {
	static bool doIt(char &value, const std::string &str) {
		if (str.size() != 1) {
			return false;
		}
		value = str[0];
		return true;
	}
};

template<>
struct ReadFromString<bool> {
	static bool doIt(bool &value, const std::string &str) {
		std::string lowStr = str;
		for (char &c: lowStr) {
			c = std::tolower(c);
		}
		if (lowStr == "true" || lowStr == "1") {
			value = true;
			return true;
		}
		if (lowStr == "false" || lowStr == "0") {
			value = false;
			return true;
		}
		return false;
	}
};


namespace Variables {
	bool isValidStartChar(char c) {
		return
			('a' <= c && c <= 'z') ||
			('A' <= c && c <= 'Z') ||
			(c == '_')
		;
	}
	
	bool isValidMiddleChar(char c) {
		return isValidStartChar(c) || ('0' <= c && c <= '9');
	}
	
	inline bool isValidName(const std::string &name) {
		if (name.empty()) {
			return false;
		}
		if (!isValidStartChar(name[0])) {
			return false;
		}
		for (auto iter = begin(name) + 1; iter != end(name); ++iter) {
			if (!isValidMiddleChar(*iter)) {
				return false;
			}
		}
		return true;
	}
}


namespace Preprocessor {

class Preprocessor {
private:
	std::map<std::string, std::string> variables_;
public:
	void define(const std::string &name, const std::string &value) {
		if (!Variables::isValidName(name)) {
			Alerts::error(StringBuilder()
				<< "Macro \"" << name << "\" has invalid name"
			);
		}
		variables_[name] = value;
	}
	
	std::string preprocess(const std::string &line) const {
		size_t len = line.size();
		std::string res = "";
		for (size_t i = 0; i < len;) {
			if (line[i] != '@') {
				res += line[i];
				++i;
				continue;
			}
			++i;
			size_t p = i;
			while (i < len && Variables::isValidMiddleChar(line[i])) {
				++i;
			}
			std::string varName = line.substr(p, i-p);
			if (i < len && line[i] == '!') {
				++i;
			}
			if (variables_.find(varName) == end(variables_)) {
				Alerts::error(StringBuilder() << "Macro \"" << varName << "\" not found");
			}
			res += variables_.at(varName);
		}
		return res;
	}
	
	Preprocessor() {
		variables_[""] = "@";
	}
};

}


namespace ValidateUtils {
	bool isRandomSeed(const std::string &str) {
		for (char c: str) {
			if (!(
				('0' <= c && c <= '9') ||
				('a' <= c && c <= 'z') ||
				('A' <= c && c <= 'Z')
			)) {
				return false;
			}
		}
		return true;
	}
}


template<typename T>
struct TypeInfo {};

#define PARMPARS_ADD_TYPE_NAME(type, name) \
	template<> \
	struct TypeInfo<type> { \
		static std::string getTypeName() { \
			return #name; \
		} \
	};

#define PARMPARS_ADD_TYPE(type) PARMPARS_ADD_TYPE_NAME(type, type)

PARMPARS_ADD_TYPE(bool)
PARMPARS_ADD_TYPE(char)
PARMPARS_ADD_TYPE(char16_t)
PARMPARS_ADD_TYPE(char32_t)
PARMPARS_ADD_TYPE(wchar_t)
PARMPARS_ADD_TYPE(signed char)
PARMPARS_ADD_TYPE(short)
PARMPARS_ADD_TYPE(unsigned short)
PARMPARS_ADD_TYPE(int)
PARMPARS_ADD_TYPE(unsigned int)
PARMPARS_ADD_TYPE(long)
PARMPARS_ADD_TYPE(unsigned long)
PARMPARS_ADD_TYPE(long long)
PARMPARS_ADD_TYPE(unsigned long long)

PARMPARS_ADD_TYPE(float)
PARMPARS_ADD_TYPE(double)
PARMPARS_ADD_TYPE(long double)

PARMPARS_ADD_TYPE_NAME(std::string, string)


class ParamParser {
private:
	struct VariableValue {
		std::string value;
		std::string lastType;
		int paramNumber;
		bool referenced;
	};
	
	bool loaded_ = false;
	std::map<std::string, VariableValue> variables_;
	Preprocessor::Preprocessor preprocessor_;
	
	void checkLoaded() {
		if (!loaded_) {
			Alerts::error("ParamParser is not loaded!");
		}
	}
	
	template<typename T, typename... ValidateMeans>
	void internalGet(const char *varName, T &res, ValidateMeans... means) {
		if (variables_.find(varName) != end(variables_)) {
			std::string typeName = TypeInfo<T>::getTypeName();
			VariableValue &varValue = variables_[varName];
			std::string preprocessedValue = preprocessor_.preprocess(varValue.value);
			if (!ReadFromString<T>::doIt(res, preprocessedValue)) {
				Alerts::error(StringBuilder()
					<< "Could not convert " << varName << " = \"" 
					<< preprocessedValue << "\" to type " << typeName
				);
			}
			if (!varValue.referenced) {
				varValue.referenced = true;
			} else {
				if (varValue.lastType != std::string(typeName)) {
					Alerts::warning(StringBuilder()
						<< "Variable " << varName << " referenced as two different "
						<< "types: " << varValue.lastType << " and " << typeName
					);
				}
			}
			varValue.lastType = typeName;
		}
		dataValidate(varName, res, means...);
	}
public:
	template<typename T, typename... ValidateMeans>
	void get(const char *varName, T &res, ValidateMeans... means) {
		checkLoaded();
		if (variables_.find(varName) == end(variables_)) {
			Alerts::error(StringBuilder()
				<< "Variable " << varName << " not found"
			);
		}
		internalGet(varName, res, means...);
	}
	
	template<typename T, typename... ValidateMeans>
	void getDefault(const char *varName, T &res, const T &defaultVal, ValidateMeans... means) {
		checkLoaded();
		res = defaultVal;
		internalGet(varName, res, means...);
	}
	
	template<typename T, typename... ValidateMeans>
	T get(const char *varName, ValidateMeans... means) {
		T res;
		get(varName, res, means...);
		return res;
	}
	
	template<typename T, typename... ValidateMeans>
	T getDefault(const char *varName, const T &defaultVal, ValidateMeans... means) {
		T res;
		getDefault(varName, res, defaultVal, means...);
		return res;
	}
	
	template<typename T>
	void define(const std::string &name, const T &value) {
		std::ostringstream os;
		os << value;
		preprocessor_.define(name, os.str());
	}
	
	void load(int argc, char *argv[]) {
		for (int idx = 1; idx < argc; ++idx) {
			std::string param = argv[idx]; 
			if (param.empty()) {
				continue;
			}
			if (ValidateUtils::isRandomSeed(param.c_str()) && idx == argc-1) {
				continue;
			}
			std::size_t splitPos = param.find('=');
			if (splitPos == std::string::npos) {
				Alerts::error(StringBuilder()
					<< "Error while parsing parameter #" << idx << " : "
					<< "\"" << param << "\" is not a valid variable!"
				);
			}
			std::string varName = param.substr(0, splitPos);
			std::string varValue = param.substr(splitPos+1);
			if (!Variables::isValidName(varName.c_str())) {
				Alerts::error(StringBuilder()
					<< "Error while parsing parameter #" << idx << " : "
					<< "\"" << varName << "\" is invalid variable name!"
				);
			}
			if (variables_.find(varName) != end(variables_)) {
				Alerts::error(StringBuilder()
					<< "Error while parsing parameter #" << idx << " : "
					<< "variable " << varName << " declared twice "
					<< "(first declared in parameter #"
					<< variables_[varName].paramNumber << ")"
				);
			}
			variables_[varName] = VariableValue {varValue, "undefined", idx, false};
		}
		loaded_ = true;
	}
	
	~ParamParser() {
		if (!loaded_) {
			return;
		}
		if (Alerts::hadError) {
			return;
		}
		for (const auto &iter: variables_) {
			const std::string &name = iter.first;
			const VariableValue &value = iter.second;
			if (!value.referenced) {
				Alerts::warning(StringBuilder()
					<< "Variable " << name << " is unused!"
				);
			}
		}
	}
};

ParamParser params;


#ifdef USE_TESTLIB
namespace TestLibAddon {
	template<typename T, class = typename std::enable_if<std::is_arithmetic<T>::value>::type>
	class GenRange {
	private:
		bool loaded_ = false;
		T left_, right_;
		
		void checkLoaded() const {
			if (!loaded_) {
				Alerts::error("GenRange is not loaded!");
			}
		}
	public:
		T generate() const {
			checkLoaded();
			if (left_ == right_) {
				return left_;
			} else {
				return rnd.next(left_, right_);
			}
		}
		
		T left() const {
			checkLoaded();
			return left_;
		}
		
		T right() const {
			checkLoaded();
			return right_;
		}
		
		GenRange() {}
		
		GenRange(const T &value)
			: loaded_(true), left_(value), right_(value) {
		}
		
		GenRange(const T &left, const T &right)
			: loaded_(true), left_(left), right_(right) {
		}
	};
	
	template<typename T>
	std::ostream &operator<<(std::ostream &os, const GenRange<T> &value) {
		return os << "[" << value.left() << "; " << value.right() << "]";
	}
	
	template<typename T>
	GenRange<T> genRange(const T &left, const T& right) {
		return GenRange<T>(left, right);
	}
	
	template<typename T>
	GenRange<T> genRange(const T &left) {
		return GenRange<T>(left, left);
	}
}

template<typename T>
struct ReadFromString<TestLibAddon::GenRange<T>> {
	static bool doIt(TestLibAddon::GenRange<T> &value, const std::string &str) {
		if (str.empty()) {
			return false;
		}
		T left, right;
		if (ReadFromString<T>::doIt(left, str)) {
			value = TestLibAddon::GenRange<T>(left, left);
			return true;
		}
		std::istringstream is(str);
		char separator, closeBracket, openBracket, needCloseBracket;
		if (!is.get(openBracket)) {
			return false;
		}
		if (openBracket == '[') {
			needCloseBracket = ']';
		} else if (openBracket == '(') {
			needCloseBracket = ')';
		} else {
			return false;
		}
		if (
			!(is >> left >> separator >> right >> closeBracket) ||
			is.get() != std::char_traits<char>::eof() ||
			(separator != ';' && separator != ',') ||
			closeBracket != needCloseBracket
		) {
			return false;
		}
		value = TestLibAddon::GenRange<T>(left, right);
		return true;
	}
};

template<typename T>
struct TypeInfo< TestLibAddon::GenRange<T> > {
	static std::string getTypeName() {
		return "GenRange<" + TypeInfo<T>::getTypeName() + ">";
	}
};

template<typename T>
struct InRange<T, TestLibAddon::GenRange<T>> {
	static bool doIt(const Range<T> &range, const TestLibAddon::GenRange<T> &value) {
		return inRange(range, value.left()) && inRange(range, value.right());
	}
};


namespace TestLibAddon {
	class GenRegex {
	private:
		bool loaded_ = false;
		std::string pattern_;
		
		void checkLoaded() const {
			if (!loaded_) {
				Alerts::error("GenRegex is not loaded!");
			}
		}
	public:
		std::string pattern() const {
			checkLoaded();
			return pattern_;
		}
		
		std::string generate() const {
			checkLoaded();
			return rnd.next(pattern_);
		}
		
		GenRegex() {}
		
		explicit GenRegex(const std::string &pattern)
			: loaded_(true), pattern_(pattern) {
		}
	};
	
	std::ostream &operator<<(std::ostream &os, const GenRegex &value) {
		return os << value.pattern();
	}
}

PARMPARS_ADD_TYPE_NAME(TestLibAddon::GenRegex, GenRegex)

template<>
struct ReadFromString<TestLibAddon::GenRegex> {
	static bool doIt(TestLibAddon::GenRegex &value, const std::string &str) {
		value = TestLibAddon::GenRegex(str);
		return true;
	}
};


namespace TestLibAddon {
	void initGenerator(int argc, char *argv[], int randomGeneratorVersion) {
		registerGen(argc, argv, randomGeneratorVersion);
		params.load(argc, argv);
	}
}

#endif

}


using ParmParsInternal::Range;
using ParmParsInternal::range;
using ParmParsInternal::inRange;
using ParmParsInternal::params;

#ifdef USE_TESTLIB
	using ParmParsInternal::TestLibAddon::GenRange;
	using ParmParsInternal::TestLibAddon::genRange;
	using ParmParsInternal::TestLibAddon::GenRegex;
	using ParmParsInternal::TestLibAddon::initGenerator;
#endif

#define DECLARE(type, name, a...) \
	type name = params.get<type>(#name, ##a);

#define DECLARE_D(type, name, def, a...) \
	type name = params.getDefault<type>(#name, (def), ##a);

#define DECLARE_GEN(type, name, a...) \
	decltype(type().generate()) name = params.get<type>(#name, ##a).generate();

#define DECLARE_GEN_D(type, name, def, a...) \
	decltype(type().generate()) name = params.getDefault<type>(#name, (def), ##a).generate();

#endif
