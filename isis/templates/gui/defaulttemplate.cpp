...

Isis::UserInterface &ui = Isis::Application::GetUserInterface();

std::cout << ui.GetInteger("INTEGER") << std::endl;

if (ui.WasEntered("DOUBLE")) {
  std::cout << ui.GetDouble("DOUBLE") << std::endl;
}
else {
  double computed = 3.14;
}

std::cout << ui.GetString("STRING") << std::endl;

std::cout << ui.GetFilename("FILE") << std::endl;

std::cout << ui.GetFilename("INPUTCUBE") << std::endl;

if (ui.WasEntered("OUTPUTCUBE")) {
  std::cout << ui.GetDouble("OUTPUTCUBE") << std::endl;
}

...
