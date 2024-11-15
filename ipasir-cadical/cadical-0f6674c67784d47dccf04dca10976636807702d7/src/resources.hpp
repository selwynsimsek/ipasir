#ifndef _resources_hpp_INCLUDED
#define _resources_hpp_INCLUDED

namespace CaDiCaL {

double absolute_real_time ();

#ifndef __MINGW32__
  double absolute_process_time ();

  size_t maximum_resident_set_size ();
  size_t current_resident_set_size ();
#endif // __MINGW32__

}

#endif // ifndef _resources_hpp_INCLUDED
