//
//  ContentView.swift
//  Ambience
//
//  Created by Patrick Hirsh on 5/26/21.
//

import SwiftUI

struct AmbienceView: View {
    var AmbienceVM: Ambience
    
    var body: some View {
        NavigationView {
            VStack {
                DeviceList(AmbienceVM: AmbienceVM)
                .navigationTitle(Text("Ambience Nodes"))
            }
        }
    }
}



struct DeviceList: View {
    var AmbienceVM: Ambience
    var body: some View {
        List {
            ForEach(AmbienceVM.devices) { device in
                NavigationLink(destination: DeviceSettings(AmbienceVM: AmbienceVM)) {
                    DeviceEntry(IP: device.IP, Name: device.Name)
                }
            }
            NavigationLink(destination: DeviceAdd(AmbienceVM: AmbienceVM)) {
                Label("Add Device", systemImage: "plus.circle")
            }
        }
    }
}

struct DeviceEntry: View {
    var IP: String
    var Name: String
    var body: some View {
        VStack {
            Text(IP)
            Text(Name)
        }
    }
}

struct DeviceSettings: View {
    var AmbienceVM: Ambience
    var body: some View {
        Text("Device Settings")
    }
}

struct DeviceAdd: View {
    var AmbienceVM: Ambience
    var body: some View {
        Text("Add a device")
    }
}

struct ContentView_Previews: PreviewProvider {
    static var previews: some View {
        let VM = Ambience()
        AmbienceView(AmbienceVM: VM)
            .preferredColorScheme(/*@START_MENU_TOKEN@*/.dark/*@END_MENU_TOKEN@*/)
        AmbienceView(AmbienceVM: VM)
            .preferredColorScheme(.light)
    }
}
