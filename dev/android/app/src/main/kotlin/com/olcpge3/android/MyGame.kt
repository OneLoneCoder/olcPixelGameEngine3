package com.olcpge3.android

import com.google.androidgamesdk.GameActivity

class MyGame : GameActivity() {
    companion object {
        init {
            System.loadLibrary("game_native")
        }
    }
}