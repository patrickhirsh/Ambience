//
//  ContentView.swift
//  Ambience
//
//  Created by Patrick Hirsh on 5/26/21.
//

import SwiftUI

struct AmbienceHomeView: View {
    var AmbienceVM: Ambience
    
    var body: some View {
        NavigationView {
            VStack {
                
                List {
                    NavigationLink(destination: DeviceSettings()) {
                        Text("This is a test device")
                    }
                    NavigationLink(destination: DeviceSettings()) {
                        Label("Add Device", systemImage: "plus.circle")
                    }
                }.navigationTitle(Text("Ambience Nodes"))
            }
        }
    }
}

struct DeviceList: View {
    var body: some View {
        List (AmbienceVM.devices) {
            
        }
    }
}



struct DeviceSettings: View {
    var body: some View {
        Text("Device Settings")
    }
}

struct ContentView_Previews: PreviewProvider {
    static var previews: some View {
        let VM = Ambience()
        AmbienceHomeView(AmbienceVM: VM)
            .preferredColorScheme(/*@START_MENU_TOKEN@*/.dark/*@END_MENU_TOKEN@*/)
        AmbienceHomeView(AmbienceVM: VM)
            .preferredColorScheme(.light)
    }
}
