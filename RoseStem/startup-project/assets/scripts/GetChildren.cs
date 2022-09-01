using Rose;

using System;

namespace Sandbox
{
    internal class GetChildren : Entity
    {
        private void printchildren(Entity entity)
        {
            foreach (Entity child in entity.Children)
            {
                Console.WriteLine($"{child.Name} is a child of {entity.Name}");
                printchildren(child);
            }
        }
        protected override void OnCreate()
        {
            printchildren(this);
        }
        protected override void OnUpdate(float ts)
        {
        }
    }
}