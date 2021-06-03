//
//  AmbienceApp.swift
//  Ambience
//
//  Created by Patrick Hirsh on 5/26/21.
//

import SwiftUI

@main
struct AmbienceApp: App {
    private let AVM = AmbienceViewModel()
    var body: some Scene {
        WindowGroup {
            HomePage(AmbienceVM: AVM)
        }
    }
}
