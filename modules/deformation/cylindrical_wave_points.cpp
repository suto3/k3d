// K-3D
// Copyright (c) 1995-2006, Timothy M. Shead
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
		\author Timothy M. Shead (tshead@k-3d.com)
		\author Romain Behar (romainbehar@yahoo.com)
*/

#include <k3dsdk/algebra.h>
#include <k3dsdk/axis.h>
#include <k3dsdk/document_plugin_factory.h>
#include <k3d-i18n-config.h>
#include <k3dsdk/measurement.h>
#include <k3dsdk/mesh_simple_deformation_modifier.h>

namespace module
{

namespace deformation
{

/////////////////////////////////////////////////////////////////////////////
// cylindrical_wave_points

class cylindrical_wave_points :
	public k3d::mesh_simple_deformation_modifier
{
	typedef k3d::mesh_simple_deformation_modifier base;

public:
	cylindrical_wave_points(k3d::iplugin_factory& Factory, k3d::idocument& Document) :
		base(Factory, Document),
		m_along(init_owner(*this) + init_name("along") + init_label(_("Along")) + init_description(_("Wave points along given axis")) + init_value(k3d::Z) + init_enumeration(k3d::axis_values())),
		m_amplitude(init_owner(*this) + init_name("amplitude") + init_label(_("Amplitude")) + init_description(_("Wave amplitude")) + init_value(5.0) + init_step_increment(0.1) + init_units(typeid(k3d::measurement::distance))),
		m_wavelength(init_owner(*this) + init_name("wavelength") + init_label(_("Wavelength")) + init_description(_("Wavelength")) + init_value(10.0) + init_step_increment(0.1) + init_units(typeid(k3d::measurement::distance))),
		m_phase(init_owner(*this) + init_name("phase") + init_label(_("Phase")) + init_description(_("Wave phase")) + init_value(0.0) + init_step_increment(k3d::radians(1.0)) + init_units(typeid(k3d::measurement::angle)))
	{
		m_mesh_selection.changed_signal().connect(make_update_mesh_slot());
		m_along.changed_signal().connect(make_update_mesh_slot());
		m_amplitude.changed_signal().connect(make_update_mesh_slot());
		m_wavelength.changed_signal().connect(make_update_mesh_slot());
		m_phase.changed_signal().connect(make_update_mesh_slot());
	}

	void on_deform_mesh(const k3d::mesh::points_t& InputPoints, const k3d::mesh::selection_t& PointSelection, k3d::mesh::points_t& OutputPoints)
	{
		const k3d::axis along = m_along.pipeline_value();
		const double amplitude = m_amplitude.pipeline_value();
		const double wavelength = m_wavelength.pipeline_value();
		const double phase = m_phase.pipeline_value();

		// Filter-out infinite frequencies ...
		if(0 == wavelength)
			return;


		const size_t point_begin = 0;
		const size_t point_end = point_begin + OutputPoints.size();
		for(size_t point = point_begin; point != point_end; ++point)
		{
			k3d::point3 position = InputPoints[point];

			const double wave_position = phase + (k3d::pi_times_2() * position[along] / wavelength);
			const double offset = amplitude * sin(wave_position);

			const k3d::vector3 direction((k3d::X != along) * position[0], (k3d::Y != along) * position[1], (k3d::Z != along) * position[2]);
			if(0 == direction.length2())
				continue;

			OutputPoints[point] = k3d::mix(InputPoints[point], InputPoints[point] + offset * k3d::normalize(direction), PointSelection[point]);
		}
	}

	static k3d::iplugin_factory& get_factory()
	{
		static k3d::document_plugin_factory<cylindrical_wave_points,
			k3d::interface_list<k3d::imesh_source,
			k3d::interface_list<k3d::imesh_sink > > > factory(
				k3d::uuid(0xfb3c4caf, 0xec534256, 0x96705718, 0xf6e5ec41),
				"CylindricalWavePoints",
				_("Applies a wave function to points along an axis"),
				"Deformation",
				k3d::iplugin_factory::STABLE);

		return factory;
	}

private:
	k3d_data(k3d::axis, immutable_name, change_signal, with_undo, local_storage, no_constraint, enumeration_property, with_serialization) m_along;
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, no_constraint, measurement_property, with_serialization) m_amplitude;
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, no_constraint, measurement_property, with_serialization) m_wavelength;
	k3d_data(double, immutable_name, change_signal, with_undo, local_storage, no_constraint, measurement_property, with_serialization) m_phase;
};

/////////////////////////////////////////////////////////////////////////////
// cylindrical_wave_points_factory

k3d::iplugin_factory& cylindrical_wave_points_factory()
{
	return cylindrical_wave_points::get_factory();
}

} // namespace deformation

} // namespace module


