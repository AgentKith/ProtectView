#pragma once

#include "player.h"

class RelayPlayer : public VideoPlayer {
    Q_OBJECT
public:
    explicit RelayPlayer(QObject *parent = nullptr);
    void start() override;
    void stop() override;
};
