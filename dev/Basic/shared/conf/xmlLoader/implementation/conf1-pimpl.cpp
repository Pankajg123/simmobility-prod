// Not copyrighted - public domain.
//
// This sample parser implementation was generated by CodeSynthesis XSD,
// an XML Schema to C++ data binding compiler. You may use it in your
// programs without any restrictions.
//

#include "conf1-pimpl.hpp"

#include <iostream>

namespace sim_mob
{
  namespace conf
  {
    // model_pimpl
    //

    void model_pimpl::
    pre ()
    {
    }

    void model_pimpl::
    id ()
    {
    }

    void model_pimpl::
    library (const ::std::string& library)
    {
      std::cout << "library: " << library << std::endl;
    }

    void model_pimpl::
    post_model ()
    {
    }

    // workgroup_pimpl
    //

    void workgroup_pimpl::
    pre ()
    {
    }

    void workgroup_pimpl::
    id (int id)
    {
      std::cout << "id: " << id << std::endl;
    }

    void workgroup_pimpl::
    post_workgroup ()
    {
    }

    // reaction_time_pimpl
    //

    void reaction_time_pimpl::
    pre ()
    {
    }

    void reaction_time_pimpl::
    type (const ::std::string& type)
    {
      std::cout << "type: " << type << std::endl;
    }

    void reaction_time_pimpl::
    mean (int mean)
    {
      std::cout << "mean: " << mean << std::endl;
    }

    void reaction_time_pimpl::
    stdev (int stdev)
    {
      std::cout << "stdev: " << stdev << std::endl;
    }

    void reaction_time_pimpl::
    post_reaction_time ()
    {
    }

    // db_connection_pimpl
    //

    void db_connection_pimpl::
    pre ()
    {
    }

    void db_connection_pimpl::
    param ()
    {
    }

    void db_connection_pimpl::
    id ()
    {
    }

    void db_connection_pimpl::
    dbtype (const ::std::string& dbtype)
    {
      std::cout << "dbtype: " << dbtype << std::endl;
    }

    void db_connection_pimpl::
    post_db_connection ()
    {
    }

    // db_param_pimpl
    //

    void db_param_pimpl::
    pre ()
    {
    }

    void db_param_pimpl::
    name (const ::std::string& name)
    {
      std::cout << "name: " << name << std::endl;
    }

    void db_param_pimpl::
    value (const ::std::string& value)
    {
      std::cout << "value: " << value << std::endl;
    }

    void db_param_pimpl::
    post_db_param ()
    {
    }

    // db_proc_mapping_pimpl
    //

    void db_proc_mapping_pimpl::
    pre ()
    {
    }

    void db_proc_mapping_pimpl::
    name (const ::std::string& name)
    {
      std::cout << "name: " << name << std::endl;
    }

    void db_proc_mapping_pimpl::
    procedure (const ::std::string& procedure)
    {
      std::cout << "procedure: " << procedure << std::endl;
    }

    void db_proc_mapping_pimpl::
    post_db_proc_mapping ()
    {
    }

    // proc_map_pimpl
    //

    void proc_map_pimpl::
    pre ()
    {
    }

    void proc_map_pimpl::
    mapping ()
    {
    }

    void proc_map_pimpl::
    id ()
    {
    }

    void proc_map_pimpl::
    post_proc_map ()
    {
    }

    // constructs_pimpl
    //

    void constructs_pimpl::
    pre ()
    {
    }

    void constructs_pimpl::
    models ()
    {
    }

    void constructs_pimpl::
    workgroup_sizes ()
    {
    }

    void constructs_pimpl::
    reaction_times ()
    {
    }

    void constructs_pimpl::
    db_connections ()
    {
    }

    void constructs_pimpl::
    db_proc_groups ()
    {
    }

    void constructs_pimpl::
    post_constructs ()
    {
    }

    // SimMobility_pimpl
    //

    void SimMobility_pimpl::
    pre ()
    {
    }

    void SimMobility_pimpl::
    constructs ()
    {
    }

    void SimMobility_pimpl::
    post_SimMobility ()
    {
    }

    // id_pimpl
    //

    void id_pimpl::
    pre ()
    {
    }

    void id_pimpl::
    post_id ()
    {
      const ::std::string& v (post_string ());

      std::cout << "id: " << v << std::endl;
    }

    // models_pimpl
    //

    void models_pimpl::
    pre ()
    {
    }

    void models_pimpl::
    lane_changing ()
    {
    }

    void models_pimpl::
    car_following ()
    {
    }

    void models_pimpl::
    intersection_driving ()
    {
    }

    void models_pimpl::
    sidewalk_movement ()
    {
    }

    void models_pimpl::
    post_models ()
    {
    }

    // workgroup_sizes_pimpl
    //

    void workgroup_sizes_pimpl::
    pre ()
    {
    }

    void workgroup_sizes_pimpl::
    agent ()
    {
    }

    void workgroup_sizes_pimpl::
    signal ()
    {
    }

    void workgroup_sizes_pimpl::
    post_workgroup_sizes ()
    {
    }

    // reaction_times_pimpl
    //

    void reaction_times_pimpl::
    pre ()
    {
    }

    void reaction_times_pimpl::
    dist1 ()
    {
    }

    void reaction_times_pimpl::
    dist2 ()
    {
    }

    void reaction_times_pimpl::
    post_reaction_times ()
    {
    }

    // db_connections_pimpl
    //

    void db_connections_pimpl::
    pre ()
    {
    }

    void db_connections_pimpl::
    connection ()
    {
    }

    void db_connections_pimpl::
    post_db_connections ()
    {
    }

    // db_proc_groups_pimpl
    //

    void db_proc_groups_pimpl::
    pre ()
    {
    }

    void db_proc_groups_pimpl::
    proc_map ()
    {
    }

    void db_proc_groups_pimpl::
    post_db_proc_groups ()
    {
    }
  }
}

