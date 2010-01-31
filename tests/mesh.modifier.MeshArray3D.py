#python

import k3d
import testing

setup = testing.setup_mesh_modifier_test("PolyCube", "MeshArray3D")

setup.modifier.layout = setup.document.new_node("TranslateArray3D")


testing.require_valid_mesh(setup.document, setup.modifier.get_property("output_mesh"))
testing.require_similar_mesh(setup.document, setup.modifier.get_property("output_mesh"), "mesh.modifier.MeshArray3D", 1)

