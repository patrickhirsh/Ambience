//
//  Previews.swift
//  Ambience
//
//  Created by Patrick Hirsh on 6/3/21.
//

import SwiftUI

struct ContentView_Previews: PreviewProvider {
    @ObservedObject static var VM = AmbienceViewModel()
    static var previews: some View {
        HomePage(AmbienceVM: VM)
            .preferredColorScheme(/*@START_MENU_TOKEN@*/.dark/*@END_MENU_TOKEN@*/)
        HomePage(AmbienceVM: VM)
            .preferredColorScheme(.light)
    }
}
