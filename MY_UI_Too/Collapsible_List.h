#ifndef MY_UI_TOO_COLLAPSIBLE_LIST_H
#define MY_UI_TOO_COLLAPSIBLE_LIST_H
#include "Widget.h"
#include <string>

namespace MY_UI_Too{

	namespace Controls{
		class Text;
		class Collapsible_List;
		
		class Collapsible_Category_BackGround : public Widget{
		public:
			Collapsible_Category_BackGround(Collapsible_List* parent);
			virtual ~Collapsible_Category_BackGround();

			virtual void Reorient_Children();
			virtual void Set_Size(Utilities::Point size) override;
			
			virtual void Add_Child(IWidget* child)override;
			virtual void Remove_Child(IWidget* child)override;// this does not call delete on the child
		};

		class Collapsible_Category : public Widget{
			MY_UI_Too::Controls::Text* _Title;
			Collapsible_Category_BackGround* _Background;
			void _Update_Mouse_Cursor();
			
		public:
			Collapsible_Category(Collapsible_List* parent);
			virtual ~Collapsible_Category();

			virtual void Reorient_Children();
			virtual void Set_Size(Utilities::Point size) override;

			virtual void Set_Title(std::string title);
			virtual std::string Get_Title() const;

			virtual void Mouse_Entered() override;
			virtual void Mouse_Moved() override;
			virtual void Draw(MY_UI_Too::Interfaces::ISkin* skin) override;

			virtual void Add_Child(IWidget* child)override;
			virtual void Remove_Child(IWidget* child)override;// this does not call delete on the child
		};


		class Collapsible_List : public Widget{
			MY_UI_Too::Controls::Text* _Title;
			MY_UI_Too::Controls::Widget* _Background;
			void _Update_Mouse_Cursor();

		public:
			Collapsible_List(IWidget* parent);
			virtual ~Collapsible_List();

			virtual void Reorient_Children();

			virtual void Set_Title(std::string title);
			virtual std::string Get_Title() const;

			virtual void Set_Size(Utilities::Point size) override;
			
			virtual void Mouse_Entered() override;
			virtual void Mouse_Moved() override;
			virtual void Draw(MY_UI_Too::Interfaces::ISkin* skin) override;

			virtual void Add_Child(IWidget* child)override;
			virtual void Remove_Child(IWidget* child)override;// this does not call delete on the child
		};
	};
};


#endif