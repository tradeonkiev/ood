#include <cstdlib>
#include "lib/Duck/BaseDuck/DecoyDuck.h"
#include "lib/Duck/BaseDuck/MallardDuck.h"
#include "lib/Duck/BaseDuck/ModelDuck.h"
#include "lib/Duck/BaseDuck/RedheadDuck.h"
#include "lib/Duck/BaseDuck/RubberDuck.h"
#include "lib/DuckFunctions.h"

int main() {
    MallardDuck mallardDuck;
    PlayWithDuck(mallardDuck);

    RedheadDuck redheadDuck;
    PlayWithDuck(redheadDuck);

    RubberDuck rubberDuck;
    PlayWithDuck(rubberDuck);

    DecoyDuck decoyDuck;
    PlayWithDuck(decoyDuck);

    ModelDuck modelDuck;
    PlayWithDuck(modelDuck);

    modelDuck.SetFlyBehavior(std::make_unique<FlyWithWings>());
    PlayWithDuck(modelDuck);

    return EXIT_SUCCESS;
}
