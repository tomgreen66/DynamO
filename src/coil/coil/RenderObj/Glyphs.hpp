/*  dynamo:- Event driven molecular dynamics simulator 
    http://www.marcusbannerman.co.uk/dynamo
    Copyright (C) 2011  Marcus N Campbell Bannerman <m.bannerman@gmail.com>

    This program is free software: you can redistribute it and/or
    modify it under the terms of the GNU General Public License
    version 3 as published by the Free Software Foundation.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <coil/RenderObj/DataSet.hpp>
#include <magnet/GL/objects/instanced.hpp>
#include <magnet/GL/objects/primitives/sphere.hpp>
#include <magnet/GL/objects/primitives/cylinder.hpp>
#include <magnet/GL/objects/primitives/arrow.hpp>

namespace coil {  
  class Glyphs : public DataSetChild, public magnet::GL::objects::Instanced
  {
  public:
    inline Glyphs(std::string name, DataSet& ds): DataSetChild(name, ds) {}

    inline virtual void clTick(const magnet::GL::Camera& cam) {}

    inline virtual void glRender(magnet::GL::FBO& fbo, const magnet::GL::Camera& cam, RenderMode mode) 
    {
      //Do not allow a glRender if uninitialised
      if (!_primitiveVertices.size()) return;

      _primitiveVertices.getContext().resetInstanceTransform();
      _positionSel->bindAttribute();
      _scaleSel->bindAttribute();
      _colorSel->bindAttribute();
      _orientSel->bindAttribute();

      Instanced::glRender();
    }

    inline virtual void init(const std::tr1::shared_ptr<magnet::thread::TaskQueue>& systemQueue)
    {
      RenderObj::init(systemQueue);            
      //Initialise the Gtk controls
      _gtkOptList.reset(new Gtk::VBox);
      _gtkOptList->show();

      Gtk::HSeparator* separator;

      //Glyph selection and level of detail
      _glyphBox.reset(new Gtk::HBox); _glyphBox->show();
      
      {
	Gtk::Label* label = Gtk::manage(new Gtk::Label("Glyph Type")); label->show();
	_glyphBox->pack_start(*label, false, false, 5);
	
	_glyphType.reset(new Gtk::ComboBoxText);
	_glyphType->show();

	_glyphType->append_text("Sphere");
	_glyphType->append_text("Arrows");
	_glyphType->append_text("Cylinder");
	_glyphType->set_active(0);

	_glyphBox->pack_start(*_glyphType, false, false, 5);
	_glyphType->signal_changed()
	  .connect(sigc::mem_fun(*this, &Glyphs::glyph_type_changed));
      }
      
      {
	_glyphLOD.reset(new Gtk::SpinButton(1.0, 0)); _glyphLOD->show();
	_glyphLOD->get_adjustment()->configure(1, 1, 1, 1.0, 1.0, 0.0); //A temporary low LOD setting
	_glyphLOD->set_numeric(true);
	_glyphBox->pack_end(*_glyphLOD, false, false, 5);
	_glyphLOD->signal_value_changed()
	  .connect(sigc::mem_fun(*this, &Glyphs::glyph_LOD_changed));

	Gtk::Label* label = Gtk::manage(new Gtk::Label("Level of Detail")); label->show();
	_glyphBox->pack_end(*label, false, false, 5);
      }

      _gtkOptList->pack_start(*_glyphBox, false, false, 5);

      separator = Gtk::manage(new Gtk::HSeparator);
      separator->show();
      _gtkOptList->pack_start(*separator, false, false, 0);

      //The attribute selectors
      _positionSel.reset(new AttributeSelector(magnet::GL::Context::instanceOriginAttrIndex,
					       false));
      
      _positionSel->buildEntries("Position Data Field:", _ds, 3, 3, Attribute::COORDINATE, 0);
      _gtkOptList->pack_start(*_positionSel, false, false);

      separator = Gtk::manage(new Gtk::HSeparator); 
      separator->show(); 
      _gtkOptList->pack_start(*separator, false, false, 0);

      _scaleSel.reset(new AttributeSelector(magnet::GL::Context::instanceScaleAttrIndex));

      _scaleSel->buildEntries("Scale Data Field:", _ds, 1, 4,
			      Attribute::INTENSIVE | Attribute::EXTENSIVE, 3);
      _gtkOptList->pack_start(*_scaleSel, false, false);

      _scaleFactorBox.reset(new Gtk::HBox);
      _scaleFactorBox->show();
      _gtkOptList->pack_start(*_scaleFactorBox, false, false, 5);
      _scaleLabel.reset(new Gtk::Label("Scale factor", 1.0, 0.5f));
      _scaleLabel->show();
      _scaleFactorBox->pack_start(*_scaleLabel, true, true, 5);
      _scaleFactor.reset(new Gtk::Entry);
      _scaleFactor->show();
      _scaleFactorBox->pack_start(*_scaleFactor, false, false, 5);
      _scaleFactor->set_text("1.0");

      _scaleFactor->signal_changed()
	.connect(sigc::mem_fun(*this, &Glyphs::glyph_scale_changed));

      separator = Gtk::manage(new Gtk::HSeparator); 
      separator->show(); 
      _gtkOptList->pack_start(*separator, false, false, 0);

      _colorSel.reset(new AttributeColorSelector);
      _colorSel->buildEntries("Color Data Field:", _ds, 1, 4, 
			      Attribute::INTENSIVE | Attribute::EXTENSIVE, 4);
      _gtkOptList->pack_start(*_colorSel, false, false);

      separator = Gtk::manage(new Gtk::HSeparator); 
      separator->show(); 
      _gtkOptList->pack_start(*separator, false, false, 0);

      _orientSel.reset(new AttributeOrientationSelector);
      _orientSel->buildEntries("Orientation Data Field:", _ds, 3, 4, 
			       Attribute::INTENSIVE | Attribute::EXTENSIVE, 4);
      _gtkOptList->pack_start(*_orientSel, false, false);

      glyph_type_changed();
    }
    
    inline virtual void deinit()
    {
      Instanced::deinit();
      RenderObj::deinit();
      _gtkOptList.reset();
      _positionSel.reset();
      _scaleSel.reset(); 
      _colorSel.reset();
      _orientSel.reset();
      _glyphType.reset();
      _glyphLOD.reset();
      _scaleFactorBox.reset();
      _scaleLabel.reset();
      _scaleFactor.reset();
    }

    inline virtual void showControls(Gtk::ScrolledWindow* win)
    {
      win->remove();
      _gtkOptList->unparent();
      win->add(*_gtkOptList);
      win->show();
    }


    inline virtual Gtk::TreeModel::iterator addViewRows(RenderObjectsGtkTreeView& view, 
							Gtk::TreeModel::iterator& parent_iter)
    {
      Gtk::TreeModel::iterator iter = view._store->append(parent_iter->children());
      (*iter)[view._columns->m_name] = getName();
      (*iter)[view._columns->m_visible] = visible();
      (*iter)[view._columns->m_shadowcasting] = shadowCasting();
      (*iter)[view._columns->m_obj] = this;
      return iter;
    }

  protected:
    
    inline void glyph_scale_changed()
    {
      magnet::gtk::forceNumericEntry(*_scaleFactor);
      glyph_type_changed();
    }


    inline void glyph_type_changed()
    {
      int type = _glyphType->get_active_row_number();
      switch (type)
	{
	case 0: //Spheres
	  _glyphLOD->get_adjustment()->configure(1.0, 0.0, 4.0, 1.0, 1.0, 0.0);
	  break;
	case 1: //Arrows
	case 2: //Cylinder
	default:
	  _glyphLOD->get_adjustment()->configure(6, 3.0, 32.0, 1.0, 5.0, 0.0);
	  break;
	}
      glyph_LOD_changed();
    }
    
    inline void glyph_LOD_changed() { Instanced::init(_ds.size()); }

    inline virtual magnet::GL::element_type::Enum  getElementType()
    { return magnet::GL::element_type::TRIANGLES; }
    
    inline virtual std::vector<GLfloat> getPrimitiveVertices()
    {
      int LOD = _glyphLOD->get_value_as_int();
      int type = _glyphType->get_active_row_number();

      switch (type)
	{
	case 0: //Spheres
	  {
	    magnet::GL::objects::primitives::Sphere sph(magnet::GL::objects::primitives::Sphere::icosahedron, LOD);
	    return std::vector<GLfloat>(sph.getVertices(), sph.getVertices() + sph.getVertexCount() * 3);
	  }
	case 1: //Arrows
	  return magnet::GL::objects::primitives::Arrow::getVertices(LOD);
	case 2: //Cylinder
	  return magnet::GL::objects::primitives::Cylinder::getVertices(LOD);
	default:
	  M_throw() << "Unrecognised glyph type";
	}
    }
    
    inline virtual std::vector<GLfloat> getPrimitiveNormals()
    {
      int LOD = _glyphLOD->get_value_as_int();
      int type = _glyphType->get_active_row_number();

      switch (type)
	{
	case 0: //Spheres
	  {
	    magnet::GL::objects::primitives::Sphere sph(magnet::GL::objects::primitives::Sphere::icosahedron, LOD);
	    return std::vector<GLfloat>(sph.getVertices(), sph.getVertices() + sph.getVertexCount() * 3);
	  }
	case 1: //Arrows
	  return magnet::GL::objects::primitives::Arrow::getNormals(LOD);
	case 2: //Cylinder
	  return magnet::GL::objects::primitives::Cylinder::getNormals(LOD);
	default:
	  M_throw() << "Unrecognised glyph type";
	}
    }
    
    inline virtual std::vector<GLuint>  getPrimitiveIndicies()
    {
      int LOD = _glyphLOD->get_value_as_int();
      int type = _glyphType->get_active_row_number();

      switch (type)
	{
	case 0: //Spheres
	  {
	    magnet::GL::objects::primitives::Sphere 
	      sph(magnet::GL::objects::primitives::Sphere::icosahedron, LOD);
	    return std::vector<GLuint>(sph.getFaces(), sph.getFaces() + sph.getFaceCount() * 3);
	  }
	case 1: //Arrows
	  return magnet::GL::objects::primitives::Arrow::getIndices(LOD);
	case 2: //Cylinder
	  return magnet::GL::objects::primitives::Cylinder::getIndices(LOD);
	default:
	  M_throw() << "Unrecognised glyph type";
	}
    }
    
    std::auto_ptr<Gtk::VBox> _gtkOptList;
    std::auto_ptr<AttributeSelector> _positionSel;
    std::auto_ptr<AttributeSelector> _scaleSel; 
    std::auto_ptr<AttributeColorSelector> _colorSel;
    std::auto_ptr<AttributeOrientationSelector> _orientSel;
    std::auto_ptr<Gtk::ComboBoxText> _glyphType;
    std::auto_ptr<Gtk::SpinButton> _glyphLOD;
    std::auto_ptr<Gtk::HBox> _glyphBox;

    std::auto_ptr<Gtk::HBox>  _scaleFactorBox;
    std::auto_ptr<Gtk::Label> _scaleLabel;
    std::auto_ptr<Gtk::Entry> _scaleFactor;
 };
}