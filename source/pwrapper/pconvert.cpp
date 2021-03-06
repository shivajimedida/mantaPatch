/******************************************************************************
 *
 * MantaFlow fluid solver framework
 * Copyright 2011 Tobias Pfaff, Nils Thuerey 
 *
 * This program is free software, distributed under the terms of the
 * GNU General Public License (GPL) 
 * http://www.gnu.org/licenses
 *
 * Python argument wrappers and conversion tools
 *
 ******************************************************************************/

#include "pythonInclude.h"
#include <sstream>
#include <algorithm>
#include "vectorbase.h"
#include "manta.h"
 
using namespace std;

//******************************************************************************
// Explicit definition and instantiation of python object converters

namespace Manta {

extern PyTypeObject PbVec3Type;
extern PyTypeObject PbVec4Type;

struct PbVec3 {
	PyObject_HEAD
	float data[3];
};

struct PbVec4 {
	PyObject_HEAD
	float data[4];
};

PyObject* getPyNone() {
	Py_INCREF(Py_None);
	return Py_None;
}
PyObject* incref(PyObject* obj) {
	Py_INCREF(obj);
	return obj;
}


/*template<> PyObject* toPy<PyObject*>(PyObject* obj) {
	return obj;     
}*/
template<> PyObject* toPy<int>( const int& v) {
	return PyLong_FromLong(v);     
}
/*template<> PyObject* toPy<char*>(const (char*) & val) {
	return PyUnicode_DecodeLatin1(val,strlen(val),"replace");
}*/
template<> PyObject* toPy<string>( const string& val) {
	return PyUnicode_DecodeLatin1(val.c_str(),val.length(),"replace");
}
template<> PyObject* toPy<float>( const float& v) {
	return PyFloat_FromDouble(v);     
}
template<> PyObject* toPy<double>( const double& v) {
	return PyFloat_FromDouble(v);     
}
template<> PyObject* toPy<bool>( const bool& v) {
	return PyBool_FromLong(v);     
}
template<> PyObject* toPy<Vec3i>(const Vec3i& v) {
	float x=(float)v.x, y=(float)v.y, z=(float)v.z;
	return PyObject_CallFunction((PyObject*)&PbVec3Type, (char*)"fff", x, y, z);
}
template<> PyObject* toPy<Vec3>(const Vec3& v) {
	float x=(float)v.x, y=(float)v.y, z=(float)v.z;
	return PyObject_CallFunction((PyObject*)&PbVec3Type, (char*)"fff", x, y, z);
}
template<> PyObject* toPy<Vec4i>(const Vec4i& v) {
	float x=(float)v.x, y=(float)v.y, z=(float)v.z;
	return PyObject_CallFunction((PyObject*)&PbVec4Type, (char*)"ffff", x, y, z);
}
template<> PyObject* toPy<Vec4>(const Vec4& v) {
	float x=(float)v.x, y=(float)v.y, z=(float)v.z;
	return PyObject_CallFunction((PyObject*)&PbVec4Type, (char*)"ffff", x, y, z);
}
template<> PyObject* toPy<PbClass*>(const PbClass_Ptr& obj) {
	return obj->getPyObject();
}

template<> float fromPy<float>(PyObject* obj) {
#if PY_MAJOR_VERSION <= 2
	if (PyInt_Check(obj)) return PyInt_AsLong(obj);
#endif
	if (PyFloat_Check(obj)) return PyFloat_AsDouble(obj);
	if (PyLong_Check(obj)) return PyLong_AsDouble(obj);
	errMsg("argument is not a float");    
}
template<> double fromPy<double>(PyObject* obj) {
#if PY_MAJOR_VERSION <= 2
	if (PyInt_Check(obj)) return PyInt_AsLong(obj);
#endif
	if (PyFloat_Check(obj)) return PyFloat_AsDouble(obj);
	if (PyLong_Check(obj)) return PyLong_AsDouble(obj);
	errMsg("argument is not a double");    
}
template<> PyObject* fromPy<PyObject*>(PyObject *obj) {
	return obj;
}
template<> int fromPy<int>(PyObject *obj) {
#if PY_MAJOR_VERSION <= 2
	if (PyInt_Check(obj)) return PyInt_AsLong(obj);
#endif
	if (PyLong_Check(obj)) return PyLong_AsDouble(obj);
	if (PyFloat_Check(obj)) {
		double a = PyFloat_AsDouble(obj);
		if (fabs(a-floor(a+0.5)) > 1e-5)
			errMsg("argument is not an int");    
		return (int) (a+0.5);
	}
	errMsg("argument is not an int");       
}
template<> string fromPy<string>(PyObject *obj) {
	if (PyUnicode_Check(obj))
		return PyBytes_AsString(PyUnicode_AsLatin1String(obj));
#if PY_MAJOR_VERSION <= 2
	else if (PyString_Check(obj))
		return PyString_AsString(obj);
#endif
	else
		errMsg("argument is not a string");
}
template<> const char* fromPy<const char*>(PyObject *obj) {
	if (PyUnicode_Check(obj))
		return PyBytes_AsString(PyUnicode_AsLatin1String(obj));
#if PY_MAJOR_VERSION <= 2
	else if (PyString_Check(obj))
		return PyString_AsString(obj);
#endif
	else errMsg("argument is not a string");
}
template<> bool fromPy<bool>(PyObject *obj) { 
	if (!PyBool_Check(obj)) errMsg("argument is not a boolean");
	return PyLong_AsLong(obj) != 0;
}
template<> Vec3 fromPy<Vec3>(PyObject* obj) {
	if (PyObject_IsInstance(obj, (PyObject*)&PbVec3Type)) {
		return Vec3(((PbVec3*)obj)->data);
	} 
	else if (PyTuple_Check(obj) && PyTuple_Size(obj) == 3) {
		return Vec3(fromPy<Real>(PyTuple_GetItem(obj,0)),
					fromPy<Real>(PyTuple_GetItem(obj,1)),
					fromPy<Real>(PyTuple_GetItem(obj,2)));
	}
	errMsg("argument is not a Vec3");
}
template<> Vec3i fromPy<Vec3i>(PyObject* obj) {
	if (PyObject_IsInstance(obj, (PyObject*)&PbVec3Type)) {
		return toVec3iChecked(((PbVec3*)obj)->data);
	}
	else if (PyTuple_Check(obj) && PyTuple_Size(obj) == 3) {
		return Vec3i(fromPy<int>(PyTuple_GetItem(obj,0)),
					 fromPy<int>(PyTuple_GetItem(obj,1)),
					 fromPy<int>(PyTuple_GetItem(obj,2)));
	}
	errMsg("argument is not a Vec3i");
}
template<> Vec4 fromPy<Vec4>(PyObject* obj) {
	if (PyObject_IsInstance(obj, (PyObject*)&PbVec4Type)) {
		return Vec4(((PbVec4*)obj)->data);
	} 
	else if (PyTuple_Check(obj) && PyTuple_Size(obj) == 4) {
		return Vec4(fromPy<Real>(PyTuple_GetItem(obj,0)),
					fromPy<Real>(PyTuple_GetItem(obj,1)),
					fromPy<Real>(PyTuple_GetItem(obj,2)),
					fromPy<Real>(PyTuple_GetItem(obj,3)));
	}
	errMsg("argument is not a Vec4");
}
template<> Vec4i fromPy<Vec4i>(PyObject* obj) {
	if (PyObject_IsInstance(obj, (PyObject*)&PbVec4Type)) {
		return toVec4i(((PbVec4*)obj)->data);
	}
	else if (PyTuple_Check(obj) && PyTuple_Size(obj) == 4) {
		return Vec4i(fromPy<int>(PyTuple_GetItem(obj,0)),
					 fromPy<int>(PyTuple_GetItem(obj,1)),
					 fromPy<int>(PyTuple_GetItem(obj,2)),
					 fromPy<int>(PyTuple_GetItem(obj,3)));
	}
	errMsg("argument is not a Vec4i");
}
template<> PbType fromPy<PbType>(PyObject* obj) {
	PbType pb = {""};
	if (!PyType_Check(obj))
		return pb;
	
	const char* tname = ((PyTypeObject*)obj)->tp_name;
	pb.S = tname;
	return pb;
}
template<> PbTypeVec fromPy<PbTypeVec>(PyObject* obj) {
	PbTypeVec vec;
	if (PyType_Check(obj)) {
		vec.T.push_back(fromPy<PbType>(obj));
	} else if (PyTuple_Check(obj)) {
		int sz = PyTuple_Size(obj);
		for (int i=0; i< sz; i++)
			vec.T.push_back(fromPy<PbType>(PyTuple_GetItem(obj,i)));
	}
	else
		errMsg("argument is not a type tuple");
	return vec;
}

template<class T> T* tmpAlloc(PyObject* obj,std::vector<void*>* tmp) {
	if (!tmp) throw Error("dynamic de-ref not supported for this type");
	void* ptr = malloc(sizeof(T));
	tmp->push_back(ptr);

	*((T*)ptr) = fromPy<T>(obj); 
	return (T*)ptr;
}
template<> float* fromPyPtr<float>(PyObject* obj, std::vector<void*>* tmp) { return tmpAlloc<float>(obj,tmp); }
template<> double* fromPyPtr<double>(PyObject* obj, std::vector<void*>* tmp) { return tmpAlloc<double>(obj,tmp); }
template<> int* fromPyPtr<int>(PyObject* obj, std::vector<void*>* tmp) { return tmpAlloc<int>(obj,tmp); }
template<> std::string* fromPyPtr<std::string>(PyObject* obj, std::vector<void*>* tmp) { return tmpAlloc<std::string>(obj,tmp); }
template<> bool* fromPyPtr<bool>(PyObject* obj, std::vector<void*>* tmp) { return tmpAlloc<bool>(obj,tmp); }
template<> Vec3* fromPyPtr<Vec3>(PyObject* obj, std::vector<void*>* tmp) { return tmpAlloc<Vec3>(obj,tmp); }
template<> Vec3i* fromPyPtr<Vec3i>(PyObject* obj, std::vector<void*>* tmp) { return tmpAlloc<Vec3i>(obj,tmp); }
template<> Vec4* fromPyPtr<Vec4>(PyObject* obj, std::vector<void*>* tmp) { return tmpAlloc<Vec4>(obj,tmp); }
template<> Vec4i* fromPyPtr<Vec4i>(PyObject* obj, std::vector<void*>* tmp) { return tmpAlloc<Vec4i>(obj,tmp); }

template<> bool isPy<float>(PyObject* obj) {
#if PY_MAJOR_VERSION <= 2
	if (PyInt_Check(obj)) return true;
#endif
	return PyFloat_Check(obj) || PyLong_Check(obj);
}
template<> bool isPy<double>(PyObject* obj) {
#if PY_MAJOR_VERSION <= 2
	if (PyInt_Check(obj)) return true;
#endif
	return PyFloat_Check(obj) || PyLong_Check(obj);
}
template<> bool isPy<PyObject*>(PyObject *obj) {
	return true;
}
template<> bool isPy<int>(PyObject *obj) {
#if PY_MAJOR_VERSION <= 2
	if (PyInt_Check(obj)) return true;
#endif
	if (PyLong_Check(obj)) return true;
	if (PyFloat_Check(obj)) {
		double a = PyFloat_AsDouble(obj);
		return fabs(a-floor(a+0.5)) < 1e-5;
	}
	return false;
}
template<> bool isPy<string>(PyObject *obj) {
	if (PyUnicode_Check(obj)) return true; 
#if PY_MAJOR_VERSION <= 2
	if (PyString_Check(obj)) return true;
#endif
	return false;
}
template<> bool isPy<const char*>(PyObject *obj) {
	if (PyUnicode_Check(obj)) return true; 
#if PY_MAJOR_VERSION <= 2
	if (PyString_Check(obj)) return true;
#endif
	return false;
}
template<> bool isPy<bool>(PyObject *obj) { 
	return PyBool_Check(obj);
}
template<> bool isPy<Vec3>(PyObject* obj) {
	if (PyObject_IsInstance(obj, (PyObject*)&PbVec3Type)) return true;
	if (PyTuple_Check(obj) && PyTuple_Size(obj) == 3) {
		return isPy<Real>(PyTuple_GetItem(obj,0)) &&
			   isPy<Real>(PyTuple_GetItem(obj,1)) &&
			   isPy<Real>(PyTuple_GetItem(obj,2));
	}
	return false;
}
template<> bool isPy<Vec3i>(PyObject* obj) {
	if (PyObject_IsInstance(obj, (PyObject*)&PbVec3Type)) return true;
	if (PyTuple_Check(obj) && PyTuple_Size(obj) == 3) {
		return isPy<int>(PyTuple_GetItem(obj,0)) &&
			   isPy<int>(PyTuple_GetItem(obj,1)) &&
			   isPy<int>(PyTuple_GetItem(obj,2));
	}
	return false;
}
template<> bool isPy<Vec4>(PyObject* obj) {
	if (PyObject_IsInstance(obj, (PyObject*)&PbVec4Type)) return true;
	if (PyTuple_Check(obj) && PyTuple_Size(obj) == 4) {
		return isPy<Real>(PyTuple_GetItem(obj,0)) &&
			   isPy<Real>(PyTuple_GetItem(obj,1)) &&
			   isPy<Real>(PyTuple_GetItem(obj,2)) &&
			   isPy<Real>(PyTuple_GetItem(obj,3));
	}
	return false;
}
template<> bool isPy<Vec4i>(PyObject* obj) {
	if (PyObject_IsInstance(obj, (PyObject*)&PbVec4Type)) return true;
	if (PyTuple_Check(obj) && PyTuple_Size(obj) == 4) {
		return isPy<int>(PyTuple_GetItem(obj,0)) &&
			   isPy<int>(PyTuple_GetItem(obj,1)) &&
			   isPy<int>(PyTuple_GetItem(obj,2)) &&
			   isPy<int>(PyTuple_GetItem(obj,3));
	}
	return false;
}
template<> bool isPy<PbType>(PyObject* obj) {
	return PyType_Check(obj);
}

//******************************************************************************
// PbArgs class defs

PbArgs PbArgs::EMPTY(NULL,NULL);

PbArgs::PbArgs(PyObject* linarg, PyObject* dict) : mLinArgs(0), mKwds(0) {
	setup(linarg, dict);
}
PbArgs::~PbArgs() {
	for(int i=0; i<(int)mTmpStorage.size(); i++)
		free(mTmpStorage[i]);
	mTmpStorage.clear();
}

void PbArgs::copy(PbArgs& a) {
	mKwds = a.mKwds;
	mData = a.mData;
	mLinData = a.mLinData;
	mLinArgs = a.mLinArgs;

}
void PbArgs::clear() {
	mLinArgs = 0;
	mKwds = 0;
	mData.clear();
	mLinData.clear();
}

PbArgs& PbArgs::operator=(const PbArgs& a) {
//    mLinArgs = 0;
//    mKwds = 0;
	return *this;
}

void PbArgs::setup(PyObject* linarg, PyObject* dict) {
	 if (dict) {
		PyObject *key, *value;
		Py_ssize_t pos = 0;
		while (PyDict_Next(dict, &pos, &key, &value)) {
			DataElement el;
			el.obj = value;
			el.visited = false;
			mData[fromPy<string>(key)] = el;
		}
		mKwds = dict;
	}
	if (linarg) {
		size_t len = PyTuple_Size(linarg);
		for (size_t i=0; i<len; i++) {
			DataElement el;
			el.obj = PyTuple_GetItem(linarg, i);
			el.visited = false;
			mLinData.push_back(el);
		}
		mLinArgs = linarg;
	}
}

void PbArgs::addLinArg(PyObject* obj) {
	DataElement el = { obj, false };
	mLinData.push_back(el);
}

void PbArgs::check() {
	if (has("nocheck")) return;
	
	for(map<string, DataElement>::iterator it = mData.begin(); it != mData.end(); it++) {
		if (!it->second.visited)
			errMsg("Argument '" + it->first + "' unknown");
	}
	for(size_t i=0; i<mLinData.size(); i++) {
		if (!mLinData[i].visited) {
			stringstream s;
			s << "Function does not read argument number #" << i;
			errMsg(s.str());
		}
	}
}

FluidSolver* PbArgs::obtainParent() {
	FluidSolver* solver = getPtrOpt<FluidSolver>("solver",-1,NULL);
	if (solver != 0) return solver;
	
	for(map<string, DataElement>::iterator it = mData.begin(); it != mData.end(); it++) {
		PbClass* obj = Pb::objFromPy(it->second.obj);

		if (obj) {
			if (solver == NULL) 
				solver = obj->getParent();
		}
	}
	for(vector<DataElement>::iterator it = mLinData.begin(); it != mLinData.end(); it++) {
		PbClass* obj = Pb::objFromPy(it->obj);
		
		if (obj) {
			if (solver == NULL) 
				solver = obj->getParent();
		}
	}
	
	return solver;    
}

void PbArgs::visit(int number, const string& key) {
	if (number >= 0 && number < (int)mLinData.size())
		mLinData[number].visited = true;
	map<string, DataElement>::iterator lu = mData.find(key);
	if (lu != mData.end()) 
		lu->second.visited = true;
}

PyObject* PbArgs::getItem(const std::string& key, bool strict, ArgLocker* lk) {
	map<string, DataElement>::iterator lu = mData.find(key);
	if (lu == mData.end()) {
		if (strict)
			errMsg ("Argument '" + key + "' is not defined.");
		return NULL;
	}
	PbClass* pbo = Pb::objFromPy(lu->second.obj);
	// try to lock
	if (pbo && lk) lk->add(pbo);        
	return lu->second.obj;
}

PyObject* PbArgs::getItem(size_t number, bool strict, ArgLocker* lk) {
	if (number >= mLinData.size()) {
		if (!strict)
			return NULL;
		stringstream s;
		s << "Argument number #" << number << " not specified.";
		errMsg(s.str());
	}
	PbClass* pbo = Pb::objFromPy(mLinData[number].obj);
	// try to lock
	if (pbo && lk) lk->add(pbo);    
	return mLinData[number].obj;
}

//******************************************************************************
// ArgLocker class defs

void ArgLocker::add(PbClass* p) {
	if (find(locks.begin(), locks.end(), p) == locks.end()) {
		locks.push_back(p);
		p->lock();
	}
}
ArgLocker::~ArgLocker() {
	for (size_t i=0; i<locks.size(); i++)
		locks[i]->unlock();
	locks.clear();
}

} // namespace
