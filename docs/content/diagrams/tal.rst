.. uml::

   @startuml
   node "Godot" {
     [Godot native classes]
     [Godot-cpp classes]
   }

   node "Sota" {
     [TAL]
     [Sota classes]
   }

   [TAL] -up-> [Godot native classes]
   [TAL] -up-> [Godot-cpp classes]
   [Sota classes] -up-> [TAL]
   @enduml
