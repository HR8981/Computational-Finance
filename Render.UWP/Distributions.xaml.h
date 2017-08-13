﻿#pragma once

#include "Distributions.g.h"

#include "EnumToBooleanConverter.h"
#include "DistributionViewModel.h"

namespace RenderEngine
{
	namespace Views
	{
		/// <summary>
		/// A view for generating random distributions of data.
		/// </summary>
		[Windows::Foundation::Metadata::WebHostHidden]
        public ref class Distributions sealed
		{
		private:
			RenderEngine::ViewModels::DistributionViewModel^ viewModel;

		public:
			Distributions();

			property RenderEngine::ViewModels::DistributionViewModel^ ViewModel
			{
				RenderEngine::ViewModels::DistributionViewModel^ get() { return this->viewModel; };
			}

		protected:

			void DrawClick(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e);
		};
	}
}