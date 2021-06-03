//
//  Ambience.swift
//  Ambience
//
//  Created by Patrick Hirsh on 6/2/21.
//

import SwiftUI

class AmbienceViewModel: ObservableObject {
    @Published var devices = [AmbienceDevice]()
}
