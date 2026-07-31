#pragma once

#include "player.h"
#include <QProcess>

class CompositePlayer : public VideoPlayer {
    Q_OBJECT
public:
    explicit CompositePlayer(QObject *parent = nullptr);
    void start() override;
    void stop() override;
};
