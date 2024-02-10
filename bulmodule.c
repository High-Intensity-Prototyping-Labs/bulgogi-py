#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include <string.h>

#include "bulgogi/inc/core.h"

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
        bul_core_s core;
        size_t     x, y;
        FILE       *file      = NULL;
        const char *filename  = NULL;
        char       *build_str = NULL;
        PyObject   *core_dict = NULL;
        PyObject   *target_dict = NULL;
        PyObject   *deps_dict = NULL;
        bul_id_t   dep_id     = BUL_MAX_ID;


        if(!PyArg_ParseTuple(args, "s", &filename)) {
                return NULL;
        }

        if(!(file = fopen(filename, "rb"))) {
                return NULL;
        }

        core = bul_core_from_file(file);

        fclose(file);

        core_dict = PyDict_New();

        for(size_t x = 0; x < core.size; x++) {
                target_dict = PyDict_New();

                for(size_t y = 0; y < core.targets[x].size; y++) {
                        dep_id = core.targets[x].deps[y];
                        deps_dict = PyDict_New();

                        PyDict_SetItemString(target_dict, core.targets[dep_id].name, deps_dict);

                        Py_DecRef(deps_dict);
                }

                PyDict_SetItemString(core_dict, core.targets[x].name, target_dict);
        }

        return core_dict;
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
        return PyModule_Create(&bulmodule);
}
