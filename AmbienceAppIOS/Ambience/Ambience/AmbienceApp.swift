//
//  AmbienceApp.swift
//  Ambience
//
//  Created by Patrick Hirsh on 5/26/21.
//

import SwiftUI

@main
struct AmbienceApp: App {
    private let VM = Ambience()
    var body: some Scene {
        WindowGroup {
            AmbienceView(AmbienceVM: VM)
        }
    }
}
