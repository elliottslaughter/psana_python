//--------------------------------------------------------------------------
// File and Version Information:
//  $Id$
//
// Description:
//  Class DataSource...
//
// Author List:
//      Andrei Salnikov
//
//------------------------------------------------------------------------

//-----------------------
// This Class's Header --
//-----------------------
#include "DataSource.h"

//-----------------
// C/C++ Headers --
//-----------------

//-------------------------------
// Collaborating Class Headers --
//-------------------------------
#include "EventIter.h"
#include "RunIter.h"
#include "StepIter.h"
#include "psana_python/Env.h"

//-----------------------------------------------------------------------
// Local Macros, Typedefs, Structures, Unions and Forward Declarations --
//-----------------------------------------------------------------------

namespace {

  // type-specific methods
  PyObject* DataSource_empty(PyObject* self, PyObject*);
  PyObject* DataSource_runs(PyObject* self, PyObject*);
  PyObject* DataSource_steps(PyObject* self, PyObject*);
  PyObject* DataSource_events(PyObject* self, PyObject*);
  PyObject* DataSource_env(PyObject* self, PyObject*);
  PyObject* DataSource_end(PyObject* self, PyObject*);

  PyMethodDef methods[] = {
    { "empty",   DataSource_empty,   METH_NOARGS, "self.empty() -> bool\n\nReturns true if data source has no associated data (\"null\" source)" },
    { "runs",    DataSource_runs,    METH_NOARGS, "self.runs() -> iterator\n\nReturns iterator for contained runs (:py:class:`RunIter`)" },
    { "steps",   DataSource_steps,   METH_NOARGS, "self.steps() -> iterator\n\nReturns iterator for contained steps (:py:class:`StepIter`)" },
    { "events",  DataSource_events,  METH_NOARGS, "self.events() -> iterator\n\nReturns iterator for contained events  (:py:class:`EventIter`)" },
    { "env",     DataSource_env,     METH_NOARGS, "self.env() -> object\n\nReturns environment object, cannot be called for \"null\" source" },
    { "end",     DataSource_end,     METH_NOARGS, "self.end() -> for data sources using random access, allows user to specify end-of-job" },
    {0, 0, 0, 0}
   };

  char typedoc[] = "Python wrapper for psana DataSource class. This class represents a data source, "
      "which is a collection of events, Steps (calib cycles), and runs. Data source provides different "
      "ways to iterate over contained object, for example you can iterate over all events from start "
      "to end, or you can iterate over steps and then over events in individual step. Collaborating "
      "classes (:py:class:`RunIter`, :py:class:`StepIter`, and :py:class:`EventIter`) provide iterators "
      "which implement different scanning algorithm, this class serves as a factory for iterators.";

}

//    ----------------------------------------
//    -- Public Function Member Definitions --
//    ----------------------------------------

void
psana_python::pyext::DataSource::initType(PyObject* module)
{
  PyTypeObject* type = BaseType::typeObject() ;
  type->tp_doc = ::typedoc;
  type->tp_methods = ::methods;

  BaseType::initType("_DataSource", module, "psana");
}

namespace {

PyObject*
DataSource_empty(PyObject* self, PyObject* )
{
  psana_python::pyext::DataSource* py_this = static_cast<psana_python::pyext::DataSource*>(self);
  return PyBool_FromLong(long(py_this->m_obj.empty()));
}

PyObject*
DataSource_runs(PyObject* self, PyObject* )
{
  psana_python::pyext::DataSource* py_this = static_cast<psana_python::pyext::DataSource*>(self);
  return psana_python::pyext::RunIter::PyObject_FromCpp(py_this->m_obj.runs());
}

PyObject*
DataSource_steps(PyObject* self, PyObject* )
{
  psana_python::pyext::DataSource* py_this = static_cast<psana_python::pyext::DataSource*>(self);
  return psana_python::pyext::StepIter::PyObject_FromCpp(py_this->m_obj.steps());
}

PyObject*
DataSource_events(PyObject* self, PyObject* )
{
  psana_python::pyext::DataSource* py_this = static_cast<psana_python::pyext::DataSource*>(self);
  return psana_python::pyext::EventIter::PyObject_FromCpp(py_this->m_obj.events());
}

PyObject*
DataSource_env(PyObject* self, PyObject* )
{
  psana_python::pyext::DataSource* py_this = static_cast<psana_python::pyext::DataSource*>(self);
  PSEnv::Env& env = py_this->m_obj.env();
  return psana_python::Env::PyObject_FromCpp(env.shared_from_this());
}

PyObject*
DataSource_end(PyObject* self, PyObject* )
{
  psana_python::pyext::DataSource* py_this = static_cast<psana_python::pyext::DataSource*>(self);
  // sequential psana uses a "read-with-no-event" to learn that
  // this is the end of the job.  that never happens naturally with
  // random-access psana.  this method gives users the ability
  // to indicate that they are done accessing the data, and causes
  // module endJob methods to be called.  It is the analog of
  // run.end().  It doesn't hurt to call this in sequential mode, but
  // it isn't necessary.  - cpo
  py_this->m_obj.events().next();
  Py_RETURN_NONE;
}

}
