#include "FileBasketController.h"

FileBasketController::FileBasketController(AppModel& model, QObject* parent)
    : QObject(parent),
    model(model)
{ }
