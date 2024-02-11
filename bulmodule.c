#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include <structmember.h>

#include <string.h>
#include <stddef.h>

#include "bulgogi/inc/core.h"

typedef struct {
        PyObject_HEAD
        int number;
} CustomObject;

typedef struct bul_py_core {
        PyObject_HEAD
        bul_core_s core;
} Core;

static PyObject *
Core_new(PyTypeObject *type, PyObject *args, PyObject *kwds) {
        Core *self;

        self = (Core*) type->tp_alloc(type, 0);
        if(self == NULL) {
                return NULL;
        }

        self->core = bul_core_init();

        return (PyObject*) self;
}

static int
Core_init(Core *self, PyObject *args, PyObject *kwds) {
        static char *kwlist[] = {"from_file", NULL};
        char* filename = NULL;
        FILE *file = NULL;

        // TODO: Make the =from_file= optional one day.
        if(!PyArg_ParseTupleAndKeywords(args, kwds, "s", &filename)) {
                return -1;
        }

        file = fopen(filename, "rb");
        if(!file) {
                return -1;
        }

        bul_core_from_file(&self->core, file);

        fclose(file);

        return 0;
}

static void
Core_dealloc(Core *self) {
        bul_core_free(&self->core);
        Py_TYPE(self)->tp_free((PyObject*) self);
}

static PyObject *
Custom_add_one(CustomObject *self, PyObject *Py_UNUSED(ignored)) {
        self->number += 1;

        Py_RETURN_NONE;
}

static PyObject *
Core_raw_targets(Core *self, PyObject *Py_UNUSED(ignored)) {

        Py_RETURN_NONE;
}

static PyObject *
Core_targets(Core *self, PyObject *Py_UNUSED(ignored)) {

        Py_RETURN_NONE;
}

static PyMemberDef Custom_members[] = {
        {"number", T_INT, offsetof(CustomObject, number), 0, "custom number"},
        {NULL},
};

static PyMethodDef Custom_methods[] = {
        {"add_one", (PyCFunction) Custom_add_one, METH_NOARGS, "Adds one to the number field"},
        {NULL},
};

static PyMethodDef Core_methods[] = {
        {"raw_targets", (PyCFunction) Core_raw_targets, METH_NOARGS, "Retrieves the list of *all* core targets."},
        {"targets", (PyCFunction) Core_targets, METH_NOARGS, "Retrieves the list of targets at the DOCUMENT root."},
        {NULL},
};

static PyTypeObject CustomType = {
        .ob_base = PyVarObject_HEAD_INIT(NULL, 0)
        .tp_name = "bul.Custom",
        .tp_doc  = PyDoc_STR("A custom object"),
        .tp_basicsize = sizeof(CustomObject),
        .tp_itemsize = 0,
        .tp_flags = Py_TPFLAGS_DEFAULT,
        .tp_new = PyType_GenericNew,
        .tp_members = Custom_members,
        .tp_methods = Custom_methods,
};

static PyTypeObject CoreType = {
        .ob_base      = PyVarObject_HEAD_INIT(NULL, 0)
        .tp_name      = "bul.Core",
        .tp_doc       = PyDoc_STR("Bulgogi Core Object"),
        .tp_basicsize = sizeof(Core),
        .tp_itemsize  = 0,
        .tp_flags     = Py_TPFLAGS_DEFAULT,
        .tp_new       = Core_new,
        .tp_init      = (initproc) Core_init,
        .tp_dealloc   = (destructor) Core_dealloc,
        .tp_methods   = Core_methods,
};

static PyObject *bul_py_system(PyObject *self, PyObject *args) {
        const char *command;
        int sts;

        if(!PyArg_ParseTuple(args, "s", &command)) {
                return NULL;
        }
        sts = system(command);
        return PyLong_FromLong(sts);
}

static PyObject *bul_py_core_from_file(PyObject *self, PyObject *args) {
        PyObject   *core_py   = NULL;
        PyObject   *dep_list  = NULL;
        PyObject   *dep_str   = NULL;

        size_t     x, y;
        bul_core_s core;
        bul_id_t   dep_id     = BUL_MAX_ID;

        FILE       *file      = NULL;
        const char *filename  = NULL;

        if(!PyArg_ParseTuple(args, "s", &filename)) {
                return NULL;
        }

        if(!(file = fopen(filename, "rb"))) {
                return NULL;
        }

        core = bul_core_init();

        bul_core_from_file(&core, file);

        fclose(file);

        core_py = PyDict_New();

        for(x = 0; x < core.size; x++) {
                dep_list = PyList_New(core.targets[x].size);

                for(y = 0; y < core.targets[x].size; y++) {
                        dep_id = core.targets[x].deps[y];

                        dep_str = PyUnicode_FromString(core.targets[dep_id].name);

                        PyList_SetItem(dep_list, y, dep_str);
                }

                PyDict_SetItemString(core_py, core.targets[x].name, dep_list);

                Py_DecRef(dep_list);
        }

        return core_py;
}

static PyMethodDef BulMethods[] = {
        {"system", bul_py_system, METH_VARARGS, "Execute a shell command."},
        {"core_from_file", bul_py_core_from_file, METH_VARARGS, "Initializes a core from a YAML file."},
        {NULL, NULL, 0, NULL},
};

static struct PyModuleDef bulmodule = {
        PyModuleDef_HEAD_INIT,
        "bulgogi",
        NULL, /* module doc */
        -1,
        BulMethods,
};

PyMODINIT_FUNC PyInit_bulgogi(void) {
        PyObject *m = NULL;

        if(PyType_Ready(&CustomType) < 0) {
                return NULL;
        }

        if(PyType_Ready(&CoreType) < 0) {
                return NULL;
        }

        m = PyModule_Create(&bulmodule);
        if(m == NULL) {
                return NULL;
        }

        Py_INCREF(&CustomType);
        if(PyModule_AddObject(m, "Custom", (PyObject*) &CustomType) < 0) {
                Py_DECREF(&CustomType);
                Py_DECREF(m);
                return NULL;
        }

        Py_INCREF(&CoreType);
        if(PyModule_AddObject(m, "Core", (PyObject*) &CoreType) < 0) {
                Py_DECREF(&CoreType);
                Py_DECREF(m);
                return NULL;
        }

        return m;
}
