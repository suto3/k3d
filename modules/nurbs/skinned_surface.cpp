
// K-3D
// Copyright (c) 1995-2004, Timothy M. Shead
//
// Contact: tshead@k-3d.com
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public
// License as published by the Free Software Foundation; either
// version 2 of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public
// License along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

/** \file
		\author Carsten Haubold (CarstenHaubold@web.de)
*/

#include <k3dsdk/document_plugin_factory.h>
#include <k3dsdk/log.h>
#include <k3dsdk/module.h>
#include <k3dsdk/node.h>
#include <k3dsdk/mesh.h>
#include <k3dsdk/mesh_source.h>
#include <k3dsdk/material_sink.h>
#include <k3dsdk/mesh_operations.h>
#include <k3dsdk/nurbs.h>
#include <k3dsdk/measurement.h>
#include <k3dsdk/selection.h>
#include <k3dsdk/data.h>
#include <k3dsdk/point3.h>
#include <k3dsdk/mesh_modifier.h>
#include <k3dsdk/mesh_selection_sink.h>
#include <k3dsdk/shared_pointer.h>

#include <iostream>
#include <vector>

#include "nurbs_patch_modifier.h"

namespace module
{

	namespace nurbs
	{
		class skinned_surface :
			public k3d::mesh_selection_sink<k3d::mesh_modifier<k3d::node > >
		{
			typedef k3d::mesh_selection_sink<k3d::mesh_modifier<k3d::node > > base;
		public:
			skinned_surface(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
				base(Factory, Document)
			{
				m_mesh_selection.changed_signal().connect(make_update_mesh_slot());
			}

			void on_create_mesh(const k3d::mesh& Input, k3d::mesh& Output)
			{
				Output = Input;
			}

			void on_update_mesh(const k3d::mesh& Input, k3d::mesh& Output)
			{
				Output = Input;

				if(!k3d::validate_nurbs_curve_groups(Output))
					return;

				merge_selection(m_mesh_selection.pipeline_value(), Output);

				std::vector<size_t> curves;

				const size_t group_begin = 0;
				const size_t group_end = group_begin + (*Output.nurbs_curve_groups->first_curves).size();
				for(size_t group = group_begin; group != group_end; ++group)
				{
					const size_t curve_begin = (*Output.nurbs_curve_groups->first_curves)[group];
					const size_t curve_end = curve_begin + (*Output.nurbs_curve_groups->curve_counts)[group];
					for(size_t curve = curve_begin; curve != curve_end; ++curve)
					{
						if((*Output.nurbs_curve_groups->curve_selection)[curve] > 0.0)
                            curves.push_back(curve);
					}
				}

				if( curves.size() < 2)
				{
					k3d::log() << error << nurbs_debug << "You need to select at least 2 curves!\n" << std::endl;
				}
				else if( curves.size() == 2 )
				{
                    nurbs_curve_modifier mod(Output);
                    mod.ruled_surface(curves.at(0), curves.at(1));
				}
				else
				{
                    nurbs_curve_modifier mod(Output);
                    mod.skinned_surface(curves);
				}

				assert_warning(k3d::validate_nurbs_curve_groups(Output));
				assert_warning(k3d::validate_nurbs_patches(Output));
			}

			static k3d::iplugin_factory& get_factory()
			{
				static k3d::document_plugin_factory<skinned_surface, k3d::interface_list<k3d::imesh_source, k3d::interface_list<k3d::imesh_sink > > > factory(
				k3d::uuid(0xe6e5899a, 0x9c445204, 0xa7758297, 0x86cd38e4),
					"NurbsSkinnedSurface",
					_("Creates a NURBS surface stretched along all selected curves"),
					"NURBS",
					k3d::iplugin_factory::EXPERIMENTAL);

				return factory;
			}

		private:
		};

		//Create connect_curve factory
		k3d::iplugin_factory& skinned_surface_factory()
		{
			return skinned_surface::get_factory();
		}

	}//namespace nurbs
} //namespace module
